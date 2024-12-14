#include "game.h"

Game i_game;

static void spawn_enemy(const zf3::Vec2D pos) {
    const int enemyIndex = zf3::get_first_inactive_bit_index(i_game.enemyActivity.bytes, gk_enemyLimit);

    if (enemyIndex == -1) {
        zf3::log_error("Failed to spawn enemy as all enemy slots are taken!");
        return;
    }

    zf3::activate_bit(i_game.enemyActivity.bytes, enemyIndex);
    i_game.enemies[enemyIndex].pos = pos;
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

    i_game.player.pos.x = zf3::get_window_size().x / 2.0f;
    i_game.player.pos.y = zf3::get_window_size().y / 2.0f;
}

void run_game_tick() {
    const zf3::Vec2D mouseCamPos = zf3::screen_to_camera_pos(zf3::get_mouse_pos());

    //
    // Player
    //
    {
        const zf3::Vec2DInt moveAxis = {
            zf3::is_key_down(zf3::KEY_D) - zf3::is_key_down(zf3::KEY_A),
            zf3::is_key_down(zf3::KEY_S) - zf3::is_key_down(zf3::KEY_W)
        };

        const float moveSpd = 3.0f;

        i_game.player.pos.x += moveAxis.x * moveSpd;
        i_game.player.pos.y += moveAxis.y * moveSpd;

        zf3::g_camera.pos = i_game.player.pos;

        if (zf3::is_mouse_button_pressed(zf3::MOUSE_BUTTON_LEFT)) {
            const zf3::Vec2D dir = zf3::calc_normal(mouseCamPos - i_game.player.pos);
            const float spd = 13.0f;
            spawn_projectile(i_game.player.pos, dir * spd);
        }

        zf3::write_to_sprite_batch(WORLD_RENDER_LAYER, PLAYER_TEX, i_game.player.pos, {0, 0, 24, 40});
    }

    //
    // Enemies
    //
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

    for (int i = 0; i < gk_enemyLimit; ++i) {
        if (!zf3::is_bit_active(i_game.enemyActivity.bytes, i)) {
            continue;
        }

        zf3::write_to_sprite_batch(WORLD_RENDER_LAYER, ENEMY_TEX, i_game.enemies[i].pos, {0, 0, 24, 36});
    }

    //
    // Projectiles
    //
    for (int i = 0; i < gk_projectileLimit; ++i) {
        if (!zf3::is_bit_active(i_game.projectileActivity.bytes, i)) {
            continue;
        }

        i_game.projectiles[i].pos += i_game.projectiles[i].vel;

        zf3::write_to_sprite_batch(WORLD_RENDER_LAYER, PROJECTILE_TEX, i_game.projectiles[i].pos, {0, 0, 6, 6});
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

    //
    // UI
    //
    zf3::write_to_sprite_batch(UI_RENDER_LAYER, CURSOR_TEX, zf3::get_mouse_pos(), {0, 0, 4, 4}, {0.5f, 0.5f}, 0.0f, {zf3::g_camera.scale, zf3::g_camera.scale});
}

void clean_game() {
}
