#include "game.h"

Game i_game;

static void spawn_player(const zf3::Vec2D pos) {
    assert(!i_game.playerActive);

    i_game.playerActive = true;
    i_game.player = {
        .pos = pos,
        .hp = gk_playerHPMax
    };
}

static zf3::RectFloat get_player_collider() {
    const zf3::Vec2D playerSize = zf3::get_assets().texSizes[PLAYER_TEX];
    const zf3::Vec2D playerTopLeft = i_game.player.pos - (playerSize / 2.0f);

    return {
        playerTopLeft.x,
        playerTopLeft.y,
        playerSize.x,
        playerSize.y
    };
}

static void hurt_player(const int dmg, const zf3::Vec2D force) {
    if (i_game.player.invTime > 0) {
        return;
    }

    i_game.player.vel += force;
    i_game.player.invTime = gk_playerInvTimeMax;

    if (i_game.player.hp <= 0) {
        i_game.playerActive = false;
    }
}

static void spawn_companion(const zf3::Vec2D pos) {
    const int companionIndex = zf3::get_first_inactive_bit_index(i_game.companionActivity.bytes, gk_companionLimit);

    if (companionIndex == -1) {
        zf3::log_error("Failed to spawn companion as all companion slots are taken!");
        return;
    }

    zf3::activate_bit(i_game.companionActivity.bytes, companionIndex);

    i_game.companions[companionIndex] = {
        .pos = pos
    };
}

static void spawn_enemy(const zf3::Vec2D pos) {
    const int enemyIndex = zf3::get_first_inactive_bit_index(i_game.enemyActivity.bytes, gk_enemyLimit);

    if (enemyIndex == -1) {
        zf3::log_error("Failed to spawn enemy as all enemy slots are taken!");
        return;
    }

    zf3::activate_bit(i_game.enemyActivity.bytes, enemyIndex);

    i_game.enemies[enemyIndex] = {
        .pos = pos
    };
}

static zf3::RectFloat get_enemy_collider(const int enemyIndex) {
    assert(enemyIndex >= 0 && enemyIndex < gk_enemyLimit);
    assert(zf3::is_bit_active(i_game.enemyActivity.bytes, enemyIndex));

    const Enemy& enemy = i_game.enemies[enemyIndex];

    const zf3::Vec2D enemySize = zf3::get_assets().texSizes[ENEMY_TEX];
    const zf3::Vec2D enemyTopLeft = enemy.pos - (enemySize / 2.0f);

    return {
        enemyTopLeft.x,
        enemyTopLeft.y,
        enemySize.x,
        enemySize.y
    };
}

static void spawn_projectile(const zf3::Vec2D pos, const zf3::Vec2D vel) {
    const int projectileIndex = zf3::get_first_inactive_bit_index(i_game.projectileActivity.bytes, gk_projectileLimit);

    if (projectileIndex == -1) {
        zf3::log_error("Failed to spawn projectile as all projectile slots are taken!");
        return;
    }

    zf3::activate_bit(i_game.projectileActivity.bytes, projectileIndex);

    i_game.projectiles[projectileIndex].pos = pos;
    i_game.projectiles[projectileIndex].vel = vel;
}

void init_game() {
    zf3::g_bgColor = {0.59f, 0.79f, 0.93f, 1.0f};
    zf3::g_camera.scale = 2.0f;
    zf3::load_render_layers(NUM_RENDER_LAYERS, UI_RENDER_LAYER);

    spawn_player({zf3::get_window_size().x / 2.0f, zf3::get_window_size().y / 2.0f});

    for (int i = 0; i < 3; ++i) {
        const float spawnOffsRange = 64.0f;
        const zf3::Vec2D spawnPos = i_game.player.pos + zf3::Vec2D(
            zf3::gen_rand_float(-spawnOffsRange, spawnOffsRange),
            zf3::gen_rand_float(-spawnOffsRange, spawnOffsRange)
        );

        spawn_companion(spawnPos);
    }
}

void run_game_tick() {
    const zf3::Vec2D mouseCamPos = zf3::screen_to_camera_pos(zf3::get_mouse_pos());

    // Update player.
    if (i_game.playerActive) {
        // Process input and movement.
        const zf3::Vec2D moveAxis = {
            static_cast<float>(zf3::is_key_down(zf3::KEY_D)) - static_cast<float>(zf3::is_key_down(zf3::KEY_A)),
            static_cast<float>(zf3::is_key_down(zf3::KEY_S)) - static_cast<float>(zf3::is_key_down(zf3::KEY_W))
        };

        const float moveSpd = 3.0f;

        const zf3::Vec2D velTarg = moveAxis * moveSpd;
        i_game.player.vel = zf3::calc_lerp(i_game.player.vel, velTarg, 0.25f);
        i_game.player.pos += i_game.player.vel;

        // Handle invincibility.
        if (i_game.player.invTime > 0) {
            --i_game.player.invTime;
        }
    }

    // Update companions.
    for (int i = 0; i < gk_companionLimit; ++i) {
        if (!zf3::is_bit_active(i_game.companionActivity.bytes, i)) {
            continue;
        }

        Companion& companion = i_game.companions[i];

        const float moveSpd = 3.0f;

        const float playerDist = zf3::calc_dist(i_game.player.pos, companion.pos);
        const float playerDistMax = 96.0f;

        const zf3::Vec2D velTarg = playerDist > playerDistMax ? zf3::calc_normal(i_game.player.pos - companion.pos) * moveSpd : zf3::Vec2D {};
        companion.vel = zf3::calc_lerp(companion.vel, velTarg, 0.25f);
        companion.pos += companion.vel;
    }

    // Process enemy spawning.
    if (i_game.enemySpawnTime > 0) {
        --i_game.enemySpawnTime;
    } else {
        const zf3::Vec2D spawnPos = {
            zf3::gen_rand_float(0.0f, zf3::get_window_size().x / 2.0f),
            zf3::gen_rand_float(0.0f, zf3::get_window_size().y / 2.0f)
        };

        spawn_enemy(spawnPos);

        i_game.enemySpawnTime = gk_enemySpawnInterval;
    }

    // Handle player-enemy collisions.
    if (i_game.playerActive) {
        const zf3::RectFloat playerCollider = get_player_collider();

        for (int i = 0; i < gk_enemyLimit; ++i) {
            if (!zf3::is_bit_active(i_game.enemyActivity.bytes, i)) {
                continue;
            }

            const zf3::RectFloat enemyCollider = get_enemy_collider(i);

            if (zf3::do_rects_intersect(playerCollider, enemyCollider)) {
                hurt_player(1, zf3::calc_normal(i_game.player.pos - i_game.enemies[i].pos) * 16.0f);
            }
        }
    }

    // Process projectile movement.
    for (int i = 0; i < gk_projectileLimit; ++i) {
        if (!zf3::is_bit_active(i_game.projectileActivity.bytes, i)) {
            continue;
        }

        i_game.projectiles[i].pos += i_game.projectiles[i].vel;
    }

    //
    // Camera
    //
    {
        // Determine the look offset.
        const float playerToMouseCamPosDist = zf3::calc_dist(i_game.player.pos, mouseCamPos);
        const zf3::Vec2D playerToMouseCamPosDir = zf3::calc_normal(mouseCamPos - i_game.player.pos);

        const float lookDist = gk_camLookDistLimit * zf3::get_min(playerToMouseCamPosDist / gk_camLookDistScalarDist, 1.0f);
        const zf3::Vec2D lookOffs = playerToMouseCamPosDir * lookDist;

        // Determine and approach the target position.
        const zf3::Vec2D targPos = i_game.player.pos + lookOffs;
        zf3::g_camera.pos = zf3::calc_lerp(zf3::g_camera.pos, targPos, 0.25f);
    }

    // Write render data.
    if (i_game.playerActive) {
        zf3::write_to_sprite_batch(WORLD_RENDER_LAYER, PLAYER_TEX, i_game.player.pos, {0, 0, 24, 40});
    }

    for (int i = 0; i < gk_companionLimit; ++i) {
        if (!zf3::is_bit_active(i_game.companionActivity.bytes, i)) {
            continue;
        }

        zf3::write_to_sprite_batch(WORLD_RENDER_LAYER, PLAYER_TEX, i_game.companions[i].pos, {0, 0, 24, 40});
    }

    for (int i = 0; i < gk_enemyLimit; ++i) {
        if (!zf3::is_bit_active(i_game.enemyActivity.bytes, i)) {
            continue;
        }

        zf3::write_to_sprite_batch(WORLD_RENDER_LAYER, ENEMY_TEX, i_game.enemies[i].pos, {0, 0, 24, 36});
    }

    for (int i = 0; i < gk_projectileLimit; ++i) {
        if (!zf3::is_bit_active(i_game.projectileActivity.bytes, i)) {
            continue;
        }

        zf3::write_to_sprite_batch(WORLD_RENDER_LAYER, PROJECTILE_TEX, i_game.projectiles[i].pos, {0, 0, 6, 6});
    }

    zf3::write_to_sprite_batch(UI_RENDER_LAYER, CURSOR_TEX, zf3::get_mouse_pos(), {0, 0, 4, 4}, {0.5f, 0.5f}, 0.0f, {zf3::g_camera.scale, zf3::g_camera.scale});
}

void clean_game() {
}
