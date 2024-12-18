#pragma once

#include <zf3.h>
#include "game_states.h"
#include "title_screen.h"
#include "world.h"
#include "enemies.h"

enum TexIndex {
    PLAYER_TEX,
    SWORD_TEX,
    ENEMY_TEX,
    PROJECTILE_TEX,
    HITBOX_TEX,
    CURSOR_TEX
};

enum SoundIndex {
    PLAYER_HURT_SND
};

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
