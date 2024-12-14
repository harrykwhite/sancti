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
    zf3::load_render_layers(zf3Data->renderer, RENDER_LAYER_CNT, UI_RENDER_LAYER);

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
            .texIndex = 0,
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
            zf3::gen_rand_float(0.0f, static_cast<float>(zf3Data->windowMeta->size.x)),
            zf3::gen_rand_float(0.0f, static_cast<float>(zf3Data->windowMeta->size.y))
        };

        spawn_enemy(spawnPos);

        i_game.enemySpawnTime = gk_enemySpawnInterval;
    }

    for (int i = 0; i < gk_enemyLimit; ++i) {
        if (!zf3::is_bit_active(i_game.enemyActivity.bytes, i)) {
            continue;
        }

        const zf3::SpriteBatchWriteData writeData = {
            .texIndex = 0,
            .pos = i_game.enemies[i].pos,
            .srcRect = {0, 0, 24, 40},
            .origin = {0.5f, 0.5f},
            .rot = 0.0f,
            .scale = {1.0f, 1.0f},
            .alpha = 1.0f
        };

        zf3::write_to_sprite_batch(zf3Data->renderer, WORLD_RENDER_LAYER, &writeData, zf3Data->assets);
    }

    //
    // UI
    //
    {
        // Write the cursor.
        const zf3::SpriteBatchWriteData writeData = {
            .texIndex = 1,
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
