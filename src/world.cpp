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
    const EnemyEntsMem& entsMem = world.enemyEntsMem;
    const EnemyEntsMemInfo& entsMemInfo = world.enemyEntsMemInfo;

    const int entIndex = zf3::get_first_inactive_bit_index(entsMem.entActivity, entsMemInfo.entLimit);

    if (entIndex == -1) {
        return false;
    }

    const int entExtIndex = zf3::get_first_inactive_bit_index(entsMem.entExtsActivities[type], entsMemInfo.entTypeMaxCnts[type]);

    if (entExtIndex == -1) {
        return false;
    }

    EnemyEnt& ent = entsMem.ents[entIndex];
    void* const entExt = get_enemy_ent_ext(entIndex, world.enemyEntsMem);

    ent = {
        .type = type,
        .extIndex = entExtIndex,
        .pos = pos
    };

    get_enemy_type_info(type).init(ent, entExt);

    assert(ent.hp > 0);

    zf3::activate_bit(entsMem.entActivity, entIndex);
    zf3::activate_bit(entsMem.entExtsActivities[type], entExtIndex);

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

    const zf3::Vec2D size = zf3::get_assets().textures.sizes[PLAYER_TEX];
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

        const zf3::Vec2D hitboxDir = zf3::calc_normal(mouseCamPos - player.pos);
        const float hitboxPosDist = 32.0f;
        const zf3::Vec2D hitboxPos = player.pos + (hitboxDir * hitboxPosDist);
        const float hitboxStrength = 9.0f;

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
    const zf3::Vec2D size = zf3::get_assets().textures.sizes[ENEMIES_TEX];
    const zf3::Vec2D topLeft = enemy.pos - (size / 2.0f);

    return {
        topLeft.x,
        topLeft.y,
        size.x,
        size.y
    };
}

static void camera_tick(zf3::Camera& cam, const Player& player, const zf3::InputManager& inputManager, const zf3::Window& window) {
    static constexpr float lk_lookDistLimit = 24.0f;
    static constexpr float lk_lookDistScalarDist = lk_lookDistLimit * 32.0f;

    // Determine the look offset.
    zf3::Vec2D lookOffs = {};

    if (player.active) {
        const zf3::Vec2D mouseCamPos = zf3::screen_to_camera_pos(inputManager.inputState.mousePos, cam, window);
        const float playerToMouseCamPosDist = zf3::calc_dist(player.pos, mouseCamPos);
        const zf3::Vec2D playerToMouseCamPosDir = zf3::calc_normal(mouseCamPos - player.pos);

        const float lookDist = lk_lookDistLimit * zf3::min(playerToMouseCamPosDist / lk_lookDistScalarDist, 1.0f);
        lookOffs = playerToMouseCamPosDir * lookDist;
    }

    // Determine and approach the target position.
    zf3::Vec2D targPos = player.pos + lookOffs; // NOTE: We use the player position even if they're inactive.
    cam.pos = zf3::lerp(cam.pos, targPos, 0.25f);
}

bool init_world(World& world, const zf3::UserGameFuncData& zf3Data) {
    zf3::zero_out(world);

    //
    // Reserve memory for enemy entities.
    //
    world.enemyEntsMemInfo = {
        .entLimit = 64
    };

    world.enemyEntsMemInfo.entTypeMaxCnts[WANDERER_ENEMY] = 32;
    world.enemyEntsMemInfo.entTypeMaxCnts[PSYCHO_ENEMY] = 32;

    if (!gen_enemy_ents_mem(world.enemyEntsMem, world.enemyEntsMemInfo)) {
        return false;
    }

    spawn_enemy(world, PSYCHO_ENEMY, zf3Data.window.size / 2.0f);
    spawn_enemy(world, WANDERER_ENEMY, zf3Data.window.size / 4.0f);

    //
    //
    //
    zf3::reset_renderer(zf3Data.renderer, NUM_WORLD_RENDER_LAYERS, UI_WORLD_RENDER_LAYER, {0.59f, 0.79f, 0.93f});

    const auto musicSrcID = zf3::add_music_src(zf3Data.musicSrcManager, 0);
    zf3::play_music_src(zf3Data.musicSrcManager, musicSrcID, 0.35f);

    spawn_player(world, zf3Data.window.size / 4.0f);
    zf3Data.renderer.cam.pos = world.player.pos;

    return true;
}

bool world_tick(World& world, const zf3::UserGameFuncData& zf3Data) {
    if (world.player.active) {
        player_tick(world.player, world.hitboxes, zf3Data.inputManager, zf3Data.renderer.cam, zf3Data.window);
    }

    for (int i = 0; i < world.enemyEntsMemInfo.entLimit; ++i) {
        if (!zf3::is_bit_active(world.enemyEntsMem.entActivity, i)) {
            continue;
        }

        EnemyEnt& enemyEnt = world.enemyEntsMem.ents[i];
        get_enemy_type_info(enemyEnt.type).tick(enemyEnt, get_enemy_ent_ext(i, world.enemyEntsMem));
    }

    camera_tick(zf3Data.renderer.cam, world.player, zf3Data.inputManager, zf3Data.window);

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
    for (int i = 0; i < world.enemyEntsMemInfo.entLimit; ++i) {
        if (!zf3::is_bit_active(world.enemyEntsMem.entActivity, i)) {
            continue;
        }

        const EnemyEnt& ent = world.enemyEntsMem.ents[i];
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
    world.enemyEntsMem.arena.clean();
}
