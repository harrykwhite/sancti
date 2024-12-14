#pragma once

#include <zf3.h>

enum RenderLayer {
    WORLD_RENDER_LAYER,
    UI_RENDER_LAYER,

    RENDER_LAYER_CNT
};

struct Player {
    zf3::Vec2D pos;
};

struct Game {
    Player player;
};

void init_game(const zf3::UserGameFuncData* const zf3Data);
void run_game_tick(const zf3::UserGameFuncData* const zf3Data);
void clean_game();
