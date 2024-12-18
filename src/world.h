#pragma once

#include <zf3.h>
#include "enemies.h"

constexpr int gk_playerInvTime = 45;
constexpr float gk_playerSwordRotOffsMax = zf3::gk_pi * 0.65f;
constexpr int gk_playerSwordChargeTimeMax = 15;
constexpr float gk_playerSwordChargeRotOffs = zf3::gk_pi * 0.15f;

constexpr int gk_enemySpawnInterval = 120;

constexpr int gk_hitboxLimit = 32;

enum WorldRenderLayer {
    ENTS_WORLD_RENDER_LAYER,
    UI_WORLD_RENDER_LAYER,

    NUM_WORLD_RENDER_LAYERS
};

struct Player {
    bool active;

    zf3::Vec2D pos;
    zf3::Vec2D vel;
    int invTime;
    int hp;

    float swordRotOffs;
    bool swordRotAxis; // 0 for negative, 1 for positive.
    int swordChargeTime;
};

enum HitboxProps {
    HITBOX_PROPS_UNDEFINED = 0,
    HITBOX_PROPS_DMG_PLAYER = 1 << 0,
    HITBOX_PROPS_DMG_ENEMY = 1 << 1
};

using HitboxPropsBitset = unsigned char;

struct Hitbox {
    zf3::RectFloat rect;
    int dmg;
    zf3::Vec2D force;
    HitboxPropsBitset props;
};

using HitboxActivityBuf = zf3::ActivityBuf<Hitbox, gk_hitboxLimit>;

struct World {
    Player player;

    EnemyEntsMem enemyEntsMem;
    EnemyEntsMemInfo enemyEntsMemInfo;
    int enemyEntSpawnTime;

    HitboxActivityBuf hitboxes;
};

bool init_world(World& world, const zf3::UserGameFuncData& zf3Data);
bool world_tick(World& world, const zf3::UserGameFuncData& zf3Data);
void clean_world(World& world);
