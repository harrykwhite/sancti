#include "world.h"

#include "game.h"

static int spawn_enemy(World& world, const zf3::Vec2D pos) {
    const int enemyIndex = zf3::get_first_inactive_bit_index(world.enemies.activity);

    if (enemyIndex != -1) {
        zf3::activate_bit(world.enemies.activity, enemyIndex);

        world.enemies[enemyIndex] = {
            .pos = pos,
            .hp = 3 // TEMP
        };
    } else {
        zf3::log_error("Failed to spawn enemy as all enemy slots are taken!");
    }

    return enemyIndex;
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
}

static void hurt_player(Player& player, const zf3::Vec2D force, zf3::SoundSrcManager& sndSrcManager) {
    if (player.invTime > 0) {
        return;
    }

    zf3::add_and_play_sound_src(sndSrcManager, PLAYER_HURT_SND);

    player.vel += force;
    player.invTime = gk_playerInvTime;
}

static zf3::RectFloat get_enemy_collider(const Enemy& enemy) {
    const zf3::Vec2D size = zf3::get_assets().textures.sizes[ENEMY_TEX];
    const zf3::Vec2D topLeft = enemy.pos - (size / 2.0f);

    return {
        topLeft.x,
        topLeft.y,
        size.x,
        size.y
    };
}

static void enemy_tick(Enemy& enemy) {
    enemy.pos += enemy.vel;
    enemy.vel = zf3::lerp(enemy.vel, {}, 0.25f);
}

static void hurt_enemy(EnemyActivityBuf& enemies, const int enemyIndex, const int dmg, const zf3::Vec2D force, zf3::SoundSrcManager& sndSrcManager) {
    Enemy& enemy = enemies[enemyIndex];

    enemy.vel += force;

    enemy.hp -= dmg;

    if (enemy.hp <= 0) {
        zf3::deactivate_bit(enemies.activity, enemyIndex);
    }
}

static void proc_player_and_enemy_collisions(Player& player, EnemyActivityBuf& enemies, zf3::SoundSrcManager& sndSrcManager) {
    const zf3::RectFloat playerCollider = get_player_collider(player);

    for (int i = 0; i < gk_enemyLimit; ++i) {
        if (!zf3::is_bit_active(enemies.activity, i)) {
            continue;
        }

        const zf3::RectFloat enemyCollider = get_enemy_collider(enemies[i]);

        if (zf3::do_rects_intersect(playerCollider, enemyCollider)) {
            hurt_player(player, zf3::calc_normal(player.pos - enemies[i].pos) * 12.0f, sndSrcManager);
        }
    }
}

static void proc_hitbox_collisions(HitboxActivityBuf& hitboxes, Player& player, EnemyActivityBuf& enemies, zf3::SoundSrcManager& sndSrcManager) {
    const zf3::RectFloat playerCollider = get_player_collider(player);

    for (int i = 0; i < gk_hitboxLimit; ++i) {
        if (!zf3::is_bit_active(hitboxes.activity, i)) {
            continue;
        }

        const Hitbox& hitbox = hitboxes[i];

        if (hitbox.props & HITBOX_PROPS_DMG_PLAYER) {
            if (zf3::do_rects_intersect(playerCollider, hitbox.rect)) {
                hurt_player(player, hitbox.force, sndSrcManager);
            }
        }
        
        if (hitbox.props & HITBOX_PROPS_DMG_ENEMY) {
            for (int j = 0; j < gk_enemyLimit; ++j) {
                if (!zf3::is_bit_active(enemies.activity, j)) {
                    continue;
                }

                const zf3::RectFloat enemyCollider = get_enemy_collider(enemies[j]);

                if (zf3::do_rects_intersect(hitbox.rect, enemyCollider)) {
                    hurt_enemy(enemies, j, hitbox.dmg, hitbox.force, sndSrcManager);
                }
            }
        }
    }
}

static void camera_tick(zf3::Camera& cam, const Player& player, const zf3::InputManager& inputManager, const zf3::Window& window) {
    static constexpr float lk_lookDistLimit = 24.0f;
    static constexpr float lk_lookDistScalarDist = lk_lookDistLimit * 32.0f;

    // Determine the look offset.
    const zf3::Vec2D mouseCamPos = zf3::screen_to_camera_pos(inputManager.inputState.mousePos, cam, window);
    const float playerToMouseCamPosDist = zf3::calc_dist(player.pos, mouseCamPos);
    const zf3::Vec2D playerToMouseCamPosDir = zf3::calc_normal(mouseCamPos - player.pos);

    const float lookDist = lk_lookDistLimit * zf3::min(playerToMouseCamPosDist / lk_lookDistScalarDist, 1.0f);
    const zf3::Vec2D lookOffs = playerToMouseCamPosDir * lookDist;

    // Determine and approach the target position.
    const zf3::Vec2D targPos = player.pos + lookOffs;
    cam.pos = zf3::lerp(cam.pos, targPos, 0.25f);
}

void init_world(World& world, const zf3::UserGameFuncData& zf3Data) {
    zf3::reset_renderer(zf3Data.renderer, NUM_WORLD_RENDER_LAYERS, UI_WORLD_RENDER_LAYER, {0.59f, 0.79f, 0.93f});

    world.player.pos = zf3Data.window.size / 4.0f;
    zf3Data.renderer.cam.pos = world.player.pos;

    const auto musicSrcID = zf3::add_music_src(zf3Data.musicSrcManager, 0);
    zf3::play_music_src(zf3Data.musicSrcManager, musicSrcID, 0.35f);
}

bool world_tick(World& world, GameState& nextGameState, const zf3::UserGameFuncData& zf3Data) {
    player_tick(world.player, world.hitboxes, zf3Data.inputManager, zf3Data.renderer.cam, zf3Data.window);

    if (world.player.invTime > 0) {
        --world.player.invTime; // NOTE: Setting this to 1 actually gives you no invincibility time, and 2 gives you 1 tick. A bit misleading?
    }

    if (world.enemySpawnTime > 0) {
        --world.enemySpawnTime;
    } else {
        const zf3::Vec2D spawnPos = {
            zf3::gen_rand_float(0.0f, zf3Data.window.size.x / 2.0f),
            zf3::gen_rand_float(0.0f, zf3Data.window.size.y / 2.0f)
        };

        spawn_enemy(world, spawnPos);

        world.enemySpawnTime = gk_enemySpawnInterval;
    }

    for (int i = 0; i < gk_enemyLimit; ++i) {
        if (!zf3::is_bit_active(world.enemies.activity.bytes, i)) {
            continue;
        }

        enemy_tick(world.enemies[i]);
    }

    proc_player_and_enemy_collisions(world.player, world.enemies, zf3Data.sndSrcManager);
    proc_hitbox_collisions(world.hitboxes, world.player, world.enemies, zf3Data.sndSrcManager);

    camera_tick(zf3Data.renderer.cam, world.player, zf3Data.inputManager, zf3Data.window);

    //
    // Writing Render Data
    //

    // Player
    {
        float alpha = 1.0f;

        if (world.player.invTime > 0) {
            alpha = (world.player.invTime & 1) == (gk_playerInvTime & 1) ? 0.5f : 0.75f;
        }

        zf3::write_to_sprite_batch(zf3Data.renderer, ENTS_WORLD_RENDER_LAYER, PLAYER_TEX, world.player.pos, {0, 0, 24, 40}, {0.5f, 0.5f}, 0.0f, {1.0f, 1.0f}, alpha);

        const zf3::Vec2D mouseCamPos = zf3::screen_to_camera_pos(zf3Data.inputManager.inputState.mousePos, zf3Data.renderer.cam, zf3Data.window);
        const float swordRot = zf3::calc_dir(world.player.pos, mouseCamPos) + world.player.swordRotOffs;
        zf3::write_to_sprite_batch(zf3Data.renderer, ENTS_WORLD_RENDER_LAYER, SWORD_TEX, world.player.pos, {0, 0, 32, 10}, {-0.1f, 0.5f}, swordRot);
    }

    // Enemies
    for (int i = 0; i < gk_enemyLimit; ++i) {
        if (!zf3::is_bit_active(world.enemies.activity.bytes, i)) {
            continue;
        }

        zf3::write_to_sprite_batch(zf3Data.renderer, ENTS_WORLD_RENDER_LAYER, ENEMY_TEX, world.enemies[i].pos, {0, 0, 24, 36});
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
    zf3::write_to_sprite_batch(zf3Data.renderer, UI_WORLD_RENDER_LAYER, CURSOR_TEX, zf3Data.inputManager.inputState.mousePos, {0, 0, 4, 4}, {0.5f, 0.5f}, 0.0f, {zf3Data.renderer.cam.scale, zf3Data.renderer.cam.scale});

    //
    //
    //
    clear_bits(world.hitboxes.activity);

    return true;
}
