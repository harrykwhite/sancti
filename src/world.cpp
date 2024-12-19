#include "world.h"

#include "game.h"
#include "sprites.h"

static void spawn_player(World& world, const zf3::Vec2D pos) {
    assert(!world.player.active);

    world.player = {
        .active = true,
        .pos = pos,
        .hp = 10 // TEMP
    };
}

static bool spawn_enemy(World& world, const EnemyType type, const zf3::Vec2D pos) {
    EnemyEntsMem& entsMem = world.enemyEntsMem;

    const int entIndex = zf3::get_first_inactive_bit_index(entsMem.ptrs.entActivity, entsMem.info.entLimit);

    if (entIndex == -1) {
        return false;
    }

    const int entExtIndex = zf3::get_first_inactive_bit_index(entsMem.ptrs.entExtsActivities[type], entsMem.info.entTypeMaxCnts[type]);

    if (entExtIndex == -1) {
        return false;
    }

    EnemyEnt& ent = entsMem.ptrs.ents[entIndex];
    void* const entExt = get_enemy_ent_ext(entIndex, entsMem);

    ent = {
        .type = type,
        .extIndex = entExtIndex,
        .pos = pos
    };

    get_enemy_type_info(type).init(ent, entExt);

    assert(ent.hp > 0);

    zf3::activate_bit(entsMem.ptrs.entActivity, entIndex);
    zf3::activate_bit(entsMem.ptrs.entExtsActivities[type], entExtIndex);

    return true;
}

static int spawn_hitbox(HitboxActivityBuf& hitboxes, const zf3::Vec2D pos, const zf3::Vec2D size, const int dmg, const zf3::Vec2D force, const HitboxPropsBitset props) {
    assert(size.x > 0 && size.y > 0);
    assert(dmg > 0);
    assert(props != HITBOX_PROPS_UNDEFINED);

    const int hitboxIndex = zf3::get_first_inactive_bit_index(hitboxes.activity);

    if (hitboxIndex != -1) {
        zf3::activate_bit(hitboxes.activity, hitboxIndex);

        hitboxes[hitboxIndex] = {
            .rect = {pos.x - (size.x / 2.0f), pos.y - (size.y / 2.0f), size.x, size.y},
            .dmg = dmg,
            .force = force,
            .props = props
        };
    } else {
        zf3::log_error("Failed to spawn hitbox as all hitbox slots are taken!");
    }

    return hitboxIndex;
}

static zf3::RectFloat get_player_collider(const Player& player) {
    assert(player.active);

    const zf3::Vec2D size = to_vec_2d(get_sprite(PLAYER_SPRITE).srcRect.get_size());
    const zf3::Vec2D topLeft = player.pos - (size / 2.0f);

    return {
        topLeft.x,
        topLeft.y,
        size.x,
        size.y
    };
}

static void player_tick(Player& player, HitboxActivityBuf& hitboxes, const zf3::InputManager& inputManager, const zf3::Camera& cam, const zf3::Window& window) {
    assert(player.active);

    //
    // Movement
    //
    const zf3::Vec2D moveAxis = {
        static_cast<float>(zf3::is_key_down(zf3::KEY_D, inputManager)) - static_cast<float>(zf3::is_key_down(zf3::KEY_A, inputManager)),
        static_cast<float>(zf3::is_key_down(zf3::KEY_S, inputManager)) - static_cast<float>(zf3::is_key_down(zf3::KEY_W, inputManager))
    };

    const float moveSpd = 3.0f;

    const zf3::Vec2D velTarg = moveAxis * moveSpd;
    player.vel = zf3::lerp(player.vel, velTarg, 0.25f);
    player.pos += player.vel;

    //
    // Sword
    //
    const zf3::Vec2D mouseCamPos = zf3::screen_to_camera_pos(inputManager.inputState.mousePos, cam, window);

    if (zf3::is_mouse_button_down(zf3::MOUSE_BUTTON_LEFT, inputManager)) {
        if (player.swordChargeTime < gk_playerSwordChargeTimeMax) {
            ++player.swordChargeTime;
        }
    } else if (zf3::is_mouse_button_released(zf3::MOUSE_BUTTON_LEFT, inputManager)) {
        player.swordChargeTime = 0;
        player.swordRotAxis = !player.swordRotAxis;

        const zf3::Vec2D hitboxDir = zf3::normalized(mouseCamPos - player.pos);
        const float hitboxPosDist = 32.0f;
        const zf3::Vec2D hitboxPos = player.pos + (hitboxDir * hitboxPosDist);
        const float hitboxStrength = 11.0f;

        spawn_hitbox(hitboxes, hitboxPos, {24.0f, 24.0f}, 1, hitboxDir * hitboxStrength, HITBOX_PROPS_DMG_ENEMY);
    }

    const float swordRotOffsTargAbs = gk_playerSwordRotOffsMax + ((static_cast<float>(player.swordChargeTime) / gk_playerSwordChargeTimeMax) * gk_playerSwordChargeRotOffs);
    const float swordRotOffsTarg = ((player.swordRotAxis ? 1 : -1) * swordRotOffsTargAbs);
    player.swordRotOffs = zf3::lerp(player.swordRotOffs, swordRotOffsTarg, 0.4f);

    //
    // Invincibility
    //
    if (player.invTime > 0) {
        --player.invTime; // NOTE: Setting this to 1 actually gives you no invincibility time, and 2 gives you 1 tick. A bit misleading?
    }
}

static void hurt_player(Player& player, const int dmg, const zf3::Vec2D force, zf3::SoundSrcManager& sndSrcManager) {
    assert(player.active);

    if (player.invTime > 0) {
        return;
    }

    zf3::add_and_play_sound_src(sndSrcManager, PLAYER_HURT_SND);

    player.vel += force;
    player.invTime = gk_playerInvTime;

    player.hp -= dmg;

    if (player.hp <= 0) {
        player.active = false;
    }
}

static zf3::RectFloat get_enemy_collider(const EnemyEnt& enemy) {
    const EnemyTypeInfo& typeInfo = get_enemy_type_info(enemy.type);
    const zf3::Vec2D size = zf3::to_vec_2d(get_sprite(typeInfo.spriteIndex).srcRect.get_size());
    const zf3::Vec2D topLeft = enemy.pos - (size / 2.0f);

    return {
        topLeft.x,
        topLeft.y,
        size.x,
        size.y
    };
}

static void hurt_enemy(EnemyEntsMem& entsMem, const int enemyIndex, const int dmg, const zf3::Vec2D force, zf3::SoundSrcManager& sndSrcManager) {
    EnemyEnt& ent = entsMem.ptrs.ents[enemyIndex];

    ent.vel += force;

    ent.hp -= dmg;

    if (ent.hp <= 0) {
        zf3::deactivate_bit(entsMem.ptrs.entActivity, enemyIndex);
    }
}

static void proc_player_enemy_collisions(Player& player, const EnemyEntsMem& enemyEntsMem, const EnemyEntsMemInfo& enemyEntsMemInfo, zf3::SoundSrcManager& sndSrcManager) {
    assert(player.active);

    const zf3::RectFloat playerCollider = get_player_collider(player);

    for (int i = 0; i < enemyEntsMemInfo.entLimit; ++i) {
        if (!zf3::is_bit_active(enemyEntsMem.ptrs.entActivity, i)) {
            continue;
        }

        const EnemyEnt& enemyEnt = enemyEntsMem.ptrs.ents[i];
        const zf3::RectFloat enemyEntCollider = get_enemy_collider(enemyEnt);

        if (zf3::do_rects_intersect(playerCollider, enemyEntCollider)) {
            hurt_player(player, 1, zf3::normalized(player.pos - enemyEnt.pos) * 14.0f, sndSrcManager);
        }
    }
}

static void proc_hitbox_collisions(HitboxActivityBuf& hitboxes, Player& player, EnemyEntsMem& enemyEntsMem, zf3::SoundSrcManager& sndSrcManager) {
    const zf3::RectFloat playerCollider = get_player_collider(player);

    for (int i = 0; i < gk_hitboxLimit; ++i) {
        if (!zf3::is_bit_active(hitboxes.activity, i)) {
            continue;
        }

        const Hitbox& hitbox = hitboxes[i];

        if (hitbox.props & HITBOX_PROPS_DMG_PLAYER) {
            if (zf3::do_rects_intersect(playerCollider, hitbox.rect)) {
                hurt_player(player, hitbox.dmg, hitbox.force, sndSrcManager);
            }
        }

        if (hitbox.props & HITBOX_PROPS_DMG_ENEMY) {
            for (int j = 0; j < enemyEntsMem.info.entLimit; ++j) {
                if (!zf3::is_bit_active(enemyEntsMem.ptrs.entActivity, j)) {
                    continue;
                }

                const zf3::RectFloat enemyCollider = get_enemy_collider(enemyEntsMem.ptrs.ents[j]);

                if (zf3::do_rects_intersect(hitbox.rect, enemyCollider)) {
                    hurt_enemy(enemyEntsMem, j, hitbox.dmg, hitbox.force, sndSrcManager);
                }
            }
        }
    }
}

static void camera_tick(zf3::Camera& cam, World& world, const zf3::InputManager& inputManager, const zf3::Window& window) {
    static constexpr float lk_lookDistLimit = 24.0f;
    static constexpr float lk_lookDistScalarDist = lk_lookDistLimit * 32.0f;

    const zf3::Pt2D camSize = zf3::get_camera_size(cam, window);

    // Determine the look offset.
    zf3::Vec2D lookOffs = {};

    if (world.player.active) {
        const zf3::Vec2D mouseCamPos = zf3::screen_to_camera_pos(inputManager.inputState.mousePos, cam, window);
        const float playerToMouseCamPosDist = zf3::calc_dist(world.player.pos, mouseCamPos);
        const zf3::Vec2D playerToMouseCamPosDir = zf3::normalized(mouseCamPos - world.player.pos);

        const float lookDist = lk_lookDistLimit * zf3::min(playerToMouseCamPosDist / lk_lookDistScalarDist, 1.0f);
        lookOffs = playerToMouseCamPosDir * lookDist;
    }

    // Determine and approach the target position.
    const zf3::Vec2D targPos = world.player.pos + lookOffs; // NOTE: We use the player position even if they're inactive.
    cam.pos = zf3::lerp(cam.pos, targPos, 0.25f);

    // Clamp position within world boundaries.
    cam.pos = {
        zf3::clamp(cam.pos.x, camSize.x / 2.0f, world.size.x - (camSize.x / 2.0f)),
        zf3::clamp(cam.pos.y, camSize.y / 2.0f, world.size.y - (camSize.y / 2.0f))
    };
}

bool init_world(World& world, const zf3::UserGameFuncData& zf3Data) {
    zf3::zero_out(world);

    // Allocate world memory arena.
    if (!world.memArena.init(gk_worldMemArenaSize)) {
        return false;
    }

    // Reserve memory for enemy entities.
    world.enemyEntsMem.info = {
        .entLimit = 64
    };

    world.enemyEntsMem.info.entTypeMaxCnts[WANDERER_ENEMY] = 32;
    world.enemyEntsMem.info.entTypeMaxCnts[PSYCHO_ENEMY] = 32;

    world.enemyEntsMem.ptrs = reserve_enemy_ents_mem(world.memArena, world.enemyEntsMem.info);

    //
    world.size = {4000, 4000};

    spawn_enemy(world, PSYCHO_ENEMY, zf3::to_vec_2d(world.size) / 2.0f);
    spawn_enemy(world, WANDERER_ENEMY, (zf3::to_vec_2d(world.size) / 2.0f) + zf3::Vec2D {64.0f, 0.0f});

    //
    zf3::reset_renderer(zf3Data.renderer, NUM_WORLD_RENDER_LAYERS, UI_WORLD_RENDER_LAYER, {0.59f, 0.79f, 0.93f});

    const auto musicSrcID = zf3::add_music_src(zf3Data.musicSrcManager, 0);
    zf3::play_music_src(zf3Data.musicSrcManager, musicSrcID, 0.35f);

    spawn_player(world, zf3::to_vec_2d(world.size) / 2.0f);
    zf3Data.renderer.cam.pos = world.player.pos;

    return true;
}

bool world_tick(World& world, const zf3::UserGameFuncData& zf3Data) {
    if (world.player.active) {
        player_tick(world.player, world.hitboxes, zf3Data.inputManager, zf3Data.renderer.cam, zf3Data.window);
        proc_player_enemy_collisions(world.player, world.enemyEntsMem, world.enemyEntsMem.info, zf3Data.sndSrcManager);
    }

    for (int i = 0; i < world.enemyEntsMem.info.entLimit; ++i) {
        if (!zf3::is_bit_active(world.enemyEntsMem.ptrs.entActivity, i)) {
            continue;
        }

        EnemyEnt& enemyEnt = world.enemyEntsMem.ptrs.ents[i];
        void* const enemyEntExt = get_enemy_ent_ext(i, world.enemyEntsMem);
        get_enemy_type_info(enemyEnt.type).tick(enemyEnt, enemyEntExt);
    }

    proc_hitbox_collisions(world.hitboxes, world.player, world.enemyEntsMem, zf3Data.sndSrcManager);

    camera_tick(zf3Data.renderer.cam, world, zf3Data.inputManager, zf3Data.window);

    //
    // Writing Render Data
    //

    // Player
    if (world.player.active) {
        float alpha = 1.0f;

        if (world.player.invTime > 0) {
            alpha = (world.player.invTime & 1) == (gk_playerInvTime & 1) ? 0.5f : 0.75f;
        }

        const Sprite& sprite = get_sprite(PLAYER_SPRITE);
        zf3::write_to_sprite_batch(zf3Data.renderer, ENTS_WORLD_RENDER_LAYER, sprite.texIndex, world.player.pos, sprite.srcRect, {0.5f, 0.5f}, 0.0f, {1.0f, 1.0f}, alpha);

        // Sword
        const Sprite swordSprite = get_sprite(SWORD_SPRITE);

        const zf3::Vec2D mouseCamPos = zf3::screen_to_camera_pos(zf3Data.inputManager.inputState.mousePos, zf3Data.renderer.cam, zf3Data.window);
        const float swordRot = zf3::calc_dir(world.player.pos, mouseCamPos) + world.player.swordRotOffs;

        zf3::write_to_sprite_batch(zf3Data.renderer, ENTS_WORLD_RENDER_LAYER, swordSprite.texIndex, world.player.pos, swordSprite.srcRect, {-0.1f, 0.5f}, swordRot);
    }

    // Enemies
    for (int i = 0; i < world.enemyEntsMem.info.entLimit; ++i) {
        if (!zf3::is_bit_active(world.enemyEntsMem.ptrs.entActivity, i)) {
            continue;
        }

        const EnemyEnt& ent = world.enemyEntsMem.ptrs.ents[i];
        const EnemyTypeInfo& typeInfo = get_enemy_type_info(ent.type);
        const Sprite& sprite = get_sprite(typeInfo.spriteIndex);
        zf3::write_to_sprite_batch(zf3Data.renderer, ENTS_WORLD_RENDER_LAYER, sprite.texIndex, ent.pos, sprite.srcRect);
    }

    // Hitboxes
    for (int i = 0; i < gk_hitboxLimit; ++i) {
        if (!zf3::is_bit_active(world.hitboxes.activity.bytes, i)) {
            continue;
        }

        const Hitbox& hitbox = world.hitboxes[i];
        zf3::write_to_sprite_batch(zf3Data.renderer, ENTS_WORLD_RENDER_LAYER, HITBOX_TEX, {hitbox.rect.x, hitbox.rect.y}, {0, 0, 1, 1}, {0.0f, 0.0f}, 0.0f, {hitbox.rect.width, hitbox.rect.height});
    }

    // Cursor
    const Sprite& cursorSprite = get_sprite(CURSOR_SPRITE);
    zf3::write_to_sprite_batch(zf3Data.renderer, UI_WORLD_RENDER_LAYER, cursorSprite.texIndex, zf3Data.inputManager.inputState.mousePos, cursorSprite.srcRect, {0.5f, 0.5f}, 0.0f, {zf3Data.renderer.cam.scale, zf3Data.renderer.cam.scale});

    clear_bits(world.hitboxes.activity);

    return true;
}

void clean_world(World& world) {
    world.memArena.clean();
}
