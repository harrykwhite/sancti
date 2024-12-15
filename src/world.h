#pragma once

#include <zf3.h>
#include "game_states.h"

constexpr int gk_playerInvTimeMax = 30;
constexpr int gk_playerHPMax = 100;

constexpr int gk_companionLimit = 8;

constexpr int gk_enemyLimit = 64;
constexpr int gk_enemySpawnInterval = 120;

constexpr int gk_projectileLimit = 512;

enum RenderLayer {
    WORLD_RENDER_LAYER,
    UI_RENDER_LAYER,

    NUM_RENDER_LAYERS
};

struct Player {
    zf3::Vec2D pos;
    zf3::Vec2D vel;
    int invTime;
    int hp;
};

struct Companion {
    zf3::Vec2D pos;
    zf3::Vec2D vel;
    int invTime;
};

struct Enemy {
    zf3::Vec2D pos;
    zf3::Vec2D vel;
};

struct Projectile {
    zf3::Vec2D pos;
    zf3::Vec2D vel;
};

struct World {
    Player player;
    bool playerActive; // NOTE: Don't clear player data until "restarting" him. Likewise for everything else.

    Companion companions[gk_companionLimit];
    zf3::Bitset<gk_companionLimit> companionActivity;

    Enemy enemies[gk_enemyLimit];
    zf3::Bitset<gk_enemyLimit> enemyActivity;
    int enemySpawnTime;

    Projectile projectiles[gk_projectileLimit];
    zf3::Bitset<gk_projectileLimit> projectileActivity;
};

void init_world(World& world);
bool world_tick(World& world, GameState& nextGameState);
