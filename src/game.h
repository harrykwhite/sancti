#pragma once

#include <zf3.h>
#include "game_states.h"
#include "title_screen.h"
#include "world.h"

enum TexIndex {
    PLAYER_TEX,
    ENEMY_TEX,
    PROJECTILE_TEX,
    CURSOR_TEX,

    TEX_INDEX_CNT
};

struct Game {
    GameState state;

    union {
        TitleScreen* titleScreen;
        World* world;
    };
};

bool init_game(const zf3::UserGameFuncData& zf3Data);
bool game_tick(const zf3::UserGameFuncData& zf3Data);
void clean_game();
