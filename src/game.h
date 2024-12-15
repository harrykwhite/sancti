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

    zf3::MemArena memArena;

    union {
        World* world;
        TitleScreen* titleScreen;
    };
};

void init_game();
void game_tick();
void clean_game();
