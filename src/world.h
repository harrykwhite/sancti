#pragma once

#include <zf3.h>
#include "game_states.h"

constexpr int gk_playerInvTime = 45;

constexpr int gk_enemyLimit = 64;
constexpr int gk_enemySpawnInterval = 120;

enum WorldRenderLayer {
    ENTS_WORLD_RENDER_LAYER,
    UI_WORLD_RENDER_LAYER,

    NUM_WORLD_RENDER_LAYERS
};

struct Player {
    zf3::Vec2D pos;
    zf3::Vec2D vel;
    int invTime;
    int hp;
};

struct Enemy {
    zf3::Vec2D pos;
    zf3::Vec2D vel;
};

using EnemyActivityBuf = zf3::ActivityBuf<Enemy, gk_enemyLimit>;

struct World {
    Player player;
    bool playerActive; // NOTE: Don't clear player data until "restarting" him. Likewise for everything else.

    EnemyActivityBuf enemies;
    int enemySpawnTime;
};

void init_world(World* const world, const zf3::UserGameFuncData* const zf3Data);
bool world_tick(World* const world, const zf3::UserGameFuncData* const zf3Data, GameState* const nextGameState);
