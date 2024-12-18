#pragma once

#include <zf3.h>
#include "sprites.h"

constexpr int gk_enemyEntsMemSize = zf3::megabytes_to_bytes(1);

enum EnemyType {
    WANDERER_ENEMY,
    PSYCHO_ENEMY,

    ENEMY_TYPE_CNT
};

struct EnemyEnt {
    EnemyType type; // NOTE: Move out?
    int extIndex;

    zf3::Vec2D pos;
    zf3::Vec2D vel;

    int hp;
};

struct EnemyTypeInfo {
    void (*init)(EnemyEnt& ent, void* const entExt);
    void (*tick)(EnemyEnt& ent, void* const entExt);
    int entExtSize;
    int entExtAlignment;
    SpriteIndex spriteIndex;
};

struct WandererEnemyEnt {
    int walkTime;
    int walkBreakTime;
};

struct PsychoEnemyEnt {
    int a;
};

struct EnemyEntsMemPtrs {
    EnemyEnt* ents;
    zf3::Byte* entActivity;

    void* entExts[ENEMY_TYPE_CNT];
    zf3::Byte* entExtsActivities[ENEMY_TYPE_CNT];
};

struct EnemyEntsMemInfo {
    int entLimit;
    int entTypeMaxCnts[ENEMY_TYPE_CNT];
};

struct EnemyEntsMem {
    EnemyEntsMemPtrs ptrs;
    EnemyEntsMemInfo info;
};

void init_enemy_types();
const EnemyTypeInfo& get_enemy_type_info(const EnemyType type);

EnemyEntsMemPtrs reserve_enemy_ents_mem(zf3::MemArena& memArena, const EnemyEntsMemInfo& memInfo);

void wanderer_init(EnemyEnt& ent, void* const entExt);
void wanderer_tick(EnemyEnt& ent, void* const entExt);

void psycho_init(EnemyEnt& ent, void* const entExt);
void psycho_tick(EnemyEnt& ent, void* const entExt);

inline void* get_enemy_ent_ext(const int entIndex, const EnemyEntsMem& mem) {
    assert(entIndex >= 0 && entIndex < mem.info.entLimit);

    const EnemyEnt& ent = mem.ptrs.ents[entIndex];
    return static_cast<zf3::Byte*>(mem.ptrs.entExts[ent.type]) + (ent.extIndex * get_enemy_type_info(ent.type).entExtSize);
}
