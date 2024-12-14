#include "game.h"

Game i_game;

void init_game(const zf3::UserGameFuncData* const zf3Data) {
    zf3Data->renderer->bgColor = {0.59f, 0.79f, 0.93f, 1.0f};
    zf3::load_render_layers(zf3Data->renderer, RENDER_LAYER_CNT, UI_RENDER_LAYER);

    //i_game.player.pos.x = zf3Data->windowMeta->size.x / 2.0f;
    //i_game.player.pos.y = zf3Data->windowMeta->size.y / 2.0f;

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

        //zf3Data->cam->pos = i_game.player.pos;

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
