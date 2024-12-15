#pragma once

#include <zf3.h>

constexpr int gk_playerInvTimeMax = 30;

constexpr int gk_companionLimit = 8;

constexpr int gk_enemyLimit = 64;
constexpr int gk_enemySpawnInterval = 120;

constexpr int gk_projectileLimit = 512;

constexpr const float gk_camLookDistLimit = 24.0f;
constexpr const float gk_camLookDistScalarDist = gk_camLookDistLimit * 32.0f;

enum TexIndex {
    PLAYER_TEX,
    ENEMY_TEX,
    PROJECTILE_TEX,
    CURSOR_TEX,

    NUM_TEX_INDEXES
};

enum RenderLayer {
    WORLD_RENDER_LAYER,
    UI_RENDER_LAYER,

    NUM_RENDER_LAYERS
};

struct Player {
    zf3::Vec2D pos;
    zf3::Vec2D vel;
    int invTime;
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

template<typename T, int CNT>
struct ActivityBuf {
    T buf[CNT];
    zf3::Bitset<CNT> activity;
};

struct Game {
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

void init_game();
void run_game_tick();
void clean_game();
