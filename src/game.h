#pragma once

#include <zf3.h>
#include "title_screen.h"
#include "world.h"

struct Game {
    union {
        TitleScreen titleScreen;
        World world;
    };

    bool inWorld;
};

bool init_game(const zf3::UserGameFuncData& zf3Data);
bool game_tick(const zf3::UserGameFuncData& zf3Data);
void clean_game();
