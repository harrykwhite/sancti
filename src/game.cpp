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

void init_game(const zf3::UserGameFuncData* const zf3Data) {
    zf3Data->renderer->bgColor = {0.59f, 0.79f, 0.93f, 1.0f};
    zf3::load_render_layers(zf3Data->renderer, NUM_RENDER_LAYERS, UI_RENDER_LAYER);

    i_game.player.pos.x = zf3Data->windowMeta->size.x / 2.0f;
    i_game.player.pos.y = zf3Data->windowMeta->size.y / 2.0f;

    zf3Data->cam->scale = 2.0f;
}

void run_game_tick(const zf3::UserGameFuncData* const zf3Data) {
    //
    // Player
    //
    {
        const zf3::Vec2DInt moveAxis = {
            zf3::is_key_down(zf3::KEY_D, zf3Data->windowMeta) - zf3::is_key_down(zf3::KEY_A, zf3Data->windowMeta),
            zf3::is_key_down(zf3::KEY_S, zf3Data->windowMeta) - zf3::is_key_down(zf3::KEY_W, zf3Data->windowMeta)
        };

        const float moveSpd = 3.0f;

        i_game.player.pos.x += moveAxis.x * moveSpd;
        i_game.player.pos.y += moveAxis.y * moveSpd;

        zf3Data->cam->pos = i_game.player.pos;

        const zf3::SpriteBatchWriteData writeData = {
            .texIndex = PLAYER_TEX,
            .pos = i_game.player.pos,
            .srcRect = {0, 0, 24, 40},
            .origin = {0.5f, 0.5f},
            .rot = 0.0f,
            .scale = {1.0f, 1.0f},
            .alpha = 1.0f
        };

        zf3::write_to_sprite_batch(zf3Data->renderer, WORLD_RENDER_LAYER, &writeData, zf3Data->assets);
    }

    //
    // Enemies
    //
    if (i_game.enemySpawnTime > 0) {
        --i_game.enemySpawnTime;
    } else {
        const zf3::Vec2D spawnPos = {
            zf3::gen_rand_float(0.0f, zf3Data->windowMeta->size.x / 2.0f),
            zf3::gen_rand_float(0.0f, zf3Data->windowMeta->size.y / 2.0f)
        };

        spawn_enemy(spawnPos);

        i_game.enemySpawnTime = gk_enemySpawnInterval;
    }

    for (int i = 0; i < gk_enemyLimit; ++i) {
        if (!zf3::is_bit_active(i_game.enemyActivity.bytes, i)) {
            continue;
        }

        const zf3::SpriteBatchWriteData writeData = {
            .texIndex = ENEMY_TEX,
            .pos = i_game.enemies[i].pos,
            .srcRect = {0, 0, 24, 36},
            .origin = {0.5f, 0.5f},
            .rot = 0.0f,
            .scale = {1.0f, 1.0f},
            .alpha = 1.0f
        };

        zf3::write_to_sprite_batch(zf3Data->renderer, WORLD_RENDER_LAYER, &writeData, zf3Data->assets);
    }

    //
    // Camera
    //
    {
        // Determine the look offset.
        const zf3::Vec2D mouseCamPos = zf3::conv_screen_to_camera_pos(zf3Data->windowMeta->inputState.mousePos, zf3Data->cam, zf3Data->windowMeta);
        const float playerToMouseCamPosDist = zf3::calc_dist(i_game.player.pos, mouseCamPos);
        const zf3::Vec2D playerToMouseCamPosDir = zf3::calc_normal(mouseCamPos - i_game.player.pos);

        const float lookDist = gk_camLookDistLimit * zf3::get_min(playerToMouseCamPosDist / gk_camLookDistScalarDist, 1.0f);
        const zf3::Vec2D lookOffs = playerToMouseCamPosDir * lookDist;

        // Determine and approach the target position.
        const zf3::Vec2D targPos = i_game.player.pos + lookOffs;
        zf3Data->cam->pos = zf3::calc_lerp(zf3Data->cam->pos, targPos, 0.25f);
    }

    //
    // UI
    //
    {
        // Write the cursor.
        const zf3::SpriteBatchWriteData writeData = {
            .texIndex = CURSOR_TEX,
            .pos = zf3Data->windowMeta->inputState.mousePos,
            .srcRect = {0, 0, 4, 4},
            .origin = {0.5f, 0.5f},
            .rot = 0.0f,
            .scale = {zf3Data->cam->scale, zf3Data->cam->scale},
            .alpha = 1.0f
        };

        zf3::write_to_sprite_batch(zf3Data->renderer, UI_RENDER_LAYER, &writeData, zf3Data->assets);
    }
}

void clean_game() {
}
