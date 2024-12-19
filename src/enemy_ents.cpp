#include "enemies.h"

EnemyEntsMemPtrs reserve_enemy_ents_mem(zf3::MemArena& memArena, const EnemyEntsMemInfo& memInfo) {
    assert(memInfo.entLimit > 0);

    EnemyEntsMemPtrs memPtrs;

    memPtrs.ents = memArena.push<EnemyEnt>(memInfo.entLimit);
    assert(memPtrs.ents);

    memPtrs.entActivity = memArena.push<zf3::Byte>(zf3::bits_to_bytes(memInfo.entLimit));
    assert(memPtrs.entActivity);

    for (int i = 0; i < ENEMY_TYPE_CNT; ++i) {
        const int typeMaxCnt = memInfo.entTypeMaxCnts[i];
        assert(typeMaxCnt >= 0 && typeMaxCnt <= memInfo.entLimit);

        if (!typeMaxCnt) {
            continue;
        }

        const EnemyTypeInfo& typeInfo = get_enemy_type_info(static_cast<EnemyType>(i));
        memPtrs.entExts[i] = memArena.push_size(typeInfo.entExtSize * typeMaxCnt, typeInfo.entExtAlignment);
        assert(memPtrs.entExts[i]);

        memPtrs.entExtsActivities[i] = memArena.push<zf3::Byte>(zf3::bits_to_bytes(typeMaxCnt));
        assert(memPtrs.entExtsActivities[i]);
    }

    return memPtrs;
}
