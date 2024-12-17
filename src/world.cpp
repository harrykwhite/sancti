#include "world.h"

#include "game.h"

constexpr const float gk_camLookDistLimit = 24.0f;
constexpr const float gk_camLookDistScalarDist = gk_camLookDistLimit * 32.0f;

/*static void spawn_player(World& world, const zf3::Vec2D pos) {
    assert(!world.playerActive);

    world.playerActive = true;
    world.player = {
        .pos = pos,
        .hp = gk_playerHPMax
    };
}

static void hurt_player(World& world, const int dmg, const zf3::Vec2D force) {
    Player& player = world.player;

    if (player.invTime > 0) {
        return;
    }

    player.vel += force;
    player.invTime = gk_playerInvTimeMax;

    if (player.hp <= 0) {
        world.playerActive = false;
    }
}

static zf3::RectFloat get_player_collider(const Player& player) {
    const zf3::Vec2D playerSize = zf3::get_assets().textures.sizes[PLAYER_TEX];
    const zf3::Vec2D playerTopLeft = player.pos - (playerSize / 2.0f);

    return {
        playerTopLeft.x,
        playerTopLeft.y,
        playerSize.x,
        playerSize.y
    };
}

static void spawn_companion(World& world, const zf3::Vec2D pos) {
    const int companionIndex = zf3::get_first_inactive_bit_index(world.companionActivity.bytes, gk_companionLimit);

    if (companionIndex == -1) {
        zf3::log_error("Failed to spawn companion as all companion slots are taken!");
        return;
    }

    zf3::activate_bit(world.companionActivity.bytes, companionIndex);

    world.companions[companionIndex] = {
        .pos = pos
    };
}

static void spawn_enemy(World& world, const zf3::Vec2D pos) {
    const int enemyIndex = zf3::get_first_inactive_bit_index(world.enemyActivity.bytes, gk_enemyLimit);

    if (enemyIndex == -1) {
        zf3::log_error("Failed to spawn enemy as all enemy slots are taken!");
        return;
    }

    zf3::activate_bit(world.enemyActivity.bytes, enemyIndex);

    world.enemies[enemyIndex] = {
        .pos = pos
    };
}

static zf3::RectFloat get_enemy_collider(const Enemy& enemy) {
    const zf3::Vec2D enemySize = zf3::get_assets().textures.sizes[ENEMY_TEX];
    const zf3::Vec2D enemyTopLeft = enemy.pos - (enemySize / 2.0f);

    return {
        enemyTopLeft.x,
        enemyTopLeft.y,
        enemySize.x,
        enemySize.y
    };
}

static void spawn_projectile(World& world, const zf3::Vec2D pos, const zf3::Vec2D vel) {
    const int projectileIndex = zf3::get_first_inactive_bit_index(world.projectileActivity.bytes, gk_projectileLimit);

    if (projectileIndex == -1) {
        zf3::log_error("Failed to spawn projectile as all projectile slots are taken!");
        return;
    }

    zf3::activate_bit(world.projectileActivity.bytes, projectileIndex);

    world.projectiles[projectileIndex].pos = pos;
    world.projectiles[projectileIndex].vel = vel;
}*/

void init_world(World& world) {
    //zf3::g_bgColor = {0.59f, 0.79f, 0.93f, 1.0f};
    //zf3::g_camera.scale = 2.0f;
    //zf3::load_render_layers(NUM_RENDER_LAYERS, UI_RENDER_LAYER);

    /*spawn_player(world, {zf3::get_window_size().x / 2.0f, zf3::get_window_size().y / 2.0f});

    for (int i = 0; i < 3; ++i) {
        const float spawnOffsRange = 64.0f;
        const zf3::Vec2D spawnPos = world.player.pos + zf3::Vec2D(
            zf3::gen_rand_float(-spawnOffsRange, spawnOffsRange),
            zf3::gen_rand_float(-spawnOffsRange, spawnOffsRange)
        );

        spawn_companion(world, spawnPos);
    }*/
}

bool world_tick(World& world, GameState& nextGameState) {
#if 0
    const zf3::Vec2D mouseCamPos = zf3::screen_to_camera_pos(zf3::get_mouse_pos());

    // Update player.
    if (world.playerActive) {
        // Process input and movement.
        const zf3::Vec2D moveAxis = {
            static_cast<float>(zf3::is_key_down(zf3::KEY_D)) - static_cast<float>(zf3::is_key_down(zf3::KEY_A)),
            static_cast<float>(zf3::is_key_down(zf3::KEY_S)) - static_cast<float>(zf3::is_key_down(zf3::KEY_W))
        };

        const float moveSpd = 3.0f;

        const zf3::Vec2D velTarg = moveAxis * moveSpd;
        world.player.vel = zf3::lerp(world.player.vel, velTarg, 0.25f);
        world.player.pos += world.player.vel;

        // Handle invincibility.
        if (world.player.invTime > 0) {
            --world.player.invTime;
        }
    }

    // Update companions.
    for (int i = 0; i < gk_companionLimit; ++i) {
        if (!zf3::is_bit_active(world.companionActivity.bytes, i)) {
            continue;
        }

        Companion& companion = world.companions[i];

        const float moveSpd = 3.0f;

        const float playerDist = zf3::calc_dist(world.player.pos, companion.pos);
        const float playerDistMax = 96.0f;

        const zf3::Vec2D velTarg = playerDist > playerDistMax ? zf3::calc_normal(world.player.pos - companion.pos) * moveSpd : zf3::Vec2D {};
        companion.vel = zf3::lerp(companion.vel, velTarg, 0.25f);
        companion.pos += companion.vel;
    }

    // Process enemy spawning.
    if (world.enemySpawnTime > 0) {
        --world.enemySpawnTime;
    } else {
        const zf3::Vec2D spawnPos = {
            zf3::gen_rand_float(0.0f, zf3::get_window_size().x / 2.0f),
            zf3::gen_rand_float(0.0f, zf3::get_window_size().y / 2.0f)
        };

        spawn_enemy(world, spawnPos);

        world.enemySpawnTime = gk_enemySpawnInterval;
    }

    // Handle player-enemy collisions.
    if (world.playerActive) {
        const zf3::RectFloat playerCollider = get_player_collider(world.player);

        for (int i = 0; i < gk_enemyLimit; ++i) {
            if (!zf3::is_bit_active(world.enemyActivity.bytes, i)) {
                continue;
            }

            const zf3::RectFloat enemyCollider = get_enemy_collider(world.enemies[i]);

            if (zf3::do_rects_intersect(playerCollider, enemyCollider)) {
                hurt_player(world, 1, zf3::calc_normal(world.player.pos - world.enemies[i].pos) * 16.0f);
            }
        }
    }

    // Process projectile movement.
    for (int i = 0; i < gk_projectileLimit; ++i) {
        if (!zf3::is_bit_active(world.projectileActivity.bytes, i)) {
            continue;
        }

        world.projectiles[i].pos += world.projectiles[i].vel;
    }

    //
    // Camera
    //
    {
        // Determine the look offset.
        const float playerToMouseCamPosDist = zf3::calc_dist(world.player.pos, mouseCamPos);
        const zf3::Vec2D playerToMouseCamPosDir = zf3::calc_normal(mouseCamPos - world.player.pos);

        const float lookDist = gk_camLookDistLimit * zf3::min(playerToMouseCamPosDist / gk_camLookDistScalarDist, 1.0f);
        const zf3::Vec2D lookOffs = playerToMouseCamPosDir * lookDist;

        // Determine and approach the target position.
        const zf3::Vec2D targPos = world.player.pos + lookOffs;
        //zf3::g_camera.pos = zf3::lerp(zf3::g_camera.pos, targPos, 0.25f);
    }

    // Write render data.
    /*
    if (world.playerActive) {
        zf3::write_to_sprite_batch(WORLD_RENDER_LAYER, PLAYER_TEX, world.player.pos, {0, 0, 24, 40});
    }

    for (int i = 0; i < gk_companionLimit; ++i) {
        if (!zf3::is_bit_active(world.companionActivity.bytes, i)) {
            continue;
        }

        zf3::write_to_sprite_batch(WORLD_RENDER_LAYER, PLAYER_TEX, world.companions[i].pos, {0, 0, 24, 40});
    }

    for (int i = 0; i < gk_enemyLimit; ++i) {
        if (!zf3::is_bit_active(world.enemyActivity.bytes, i)) {
            continue;
        }

        zf3::write_to_sprite_batch(WORLD_RENDER_LAYER, ENEMY_TEX, world.enemies[i].pos, {0, 0, 24, 36});
    }

    for (int i = 0; i < gk_projectileLimit; ++i) {
        if (!zf3::is_bit_active(world.projectileActivity.bytes, i)) {
            continue;
        }

        zf3::write_to_sprite_batch(WORLD_RENDER_LAYER, PROJECTILE_TEX, world.projectiles[i].pos, {0, 0, 6, 6});
    }

    zf3::write_to_sprite_batch(UI_RENDER_LAYER, CURSOR_TEX, zf3::get_mouse_pos(), {0, 0, 4, 4}, {0.5f, 0.5f}, 0.0f, {zf3::g_camera.scale, zf3::g_camera.scale});
    */
#endif
    return true;
}
