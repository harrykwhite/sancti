#pragma once

#include <zf3.h>

constexpr int gk_enemyLimit = 64;
constexpr int gk_enemySpawnInterval = 120;

enum RenderLayer {
    WORLD_RENDER_LAYER,
    UI_RENDER_LAYER,

    RENDER_LAYER_CNT
};

struct Player {
    zf3::Vec2D pos;
};

struct Enemy {
    zf3::Vec2D pos;
};

struct Game {
    Player player;

    Enemy enemies[gk_enemyLimit];
    zf3::Bitset<gk_enemyLimit> enemyActivity;
    int enemySpawnTime;
};

void init_game(const zf3::UserGameFuncData* const zf3Data);
void run_game_tick(const zf3::UserGameFuncData* const zf3Data);
void clean_game();
