#include "enemies.h"

static EnemyTypeInfo i_enemyTypeInfos[ENEMY_TYPE_CNT];
static bool i_enemyTypeInfosInitialized;

void init_enemy_types() {
    assert(!i_enemyTypeInfosInitialized);

    i_enemyTypeInfos[WANDERER_ENEMY] = {
        .init = wanderer_init,
        .tick = wanderer_tick,
        .entExtSize = sizeof(WandererEnemyEnt),
        .entExtAlignment = alignof(WandererEnemyEnt),
        .spriteIndex = WANDERER_ENEMY_SPRITE
    };

    i_enemyTypeInfos[PSYCHO_ENEMY] = {
        .init = psycho_init,
        .tick = psycho_tick,
        .entExtSize = sizeof(PsychoEnemyEnt),
        .entExtAlignment = alignof(PsychoEnemyEnt),
        .spriteIndex = PSYCHO_ENEMY_SPRITE
    };

    i_enemyTypeInfosInitialized = true;
}

const EnemyTypeInfo& get_enemy_type_info(const EnemyType type) {
    assert(i_enemyTypeInfosInitialized);
    return i_enemyTypeInfos[type];
}
