#include "enemies.h"

bool gen_enemy_ents_mem(EnemyEntsMem& mem, const EnemyEntsMemInfo& memInfo) {
    assert(memInfo.entLimit > 0);

    if (!mem.arena.init(gk_enemyEntsMemSize)) {
        return false;
    }

    mem.ents = mem.arena.push<EnemyEnt>(memInfo.entLimit);

    if (!mem.ents) {
        mem.arena.clean();
        return false;
    }

    mem.entActivity = mem.arena.push<zf3::Byte>(zf3::bits_to_bytes(memInfo.entLimit));

    if (!mem.entActivity) {
        mem.arena.clean();
        return false;
    }

    for (int i = 0; i < ENEMY_TYPE_CNT; ++i) {
        const int typeMaxCnt = memInfo.entTypeMaxCnts[i];

        assert(typeMaxCnt >= 0 && typeMaxCnt <= memInfo.entLimit);

        if (!typeMaxCnt) {
            continue;
        }

        const EnemyTypeInfo& typeInfo = get_enemy_type_info(static_cast<EnemyType>(i));
        mem.entExts[i] = mem.arena.push_size(typeInfo.entExtSize * typeMaxCnt, typeInfo.entExtAlignment);

        if (!mem.entExts[i]) {
            mem.arena.clean();
            return false;
        }

        mem.entExtsActivities[i] = mem.arena.push<zf3::Byte>(zf3::bits_to_bytes(typeMaxCnt));

        if (!mem.entExtsActivities[i]) {
            mem.arena.clean();
            return false;
        }
    }

    return true;
}
