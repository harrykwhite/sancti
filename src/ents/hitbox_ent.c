#include "ent_types.h"

#include "../sprites.h"

static bool enemy_ent_collision_filter(const ZF4EntID entID, const ZF4EntManager* const entManager) {
    return zf4_get_ent(entManager, entID)->typeIndex == ENEMY_ENT;
}

bool init_hitbox_ent(ZF4Scene* const scene, const ZF4EntID entID, const ZF4GamePtrs* const gamePtrs) {
    ZF4Ent* const ent = zf4_get_ent(&scene->entManager, entID);
    ent->spriteIndex = BLUE_PIXEL_SPRITE;

    return true;
}

bool hitbox_ent_tick(ZF4Scene* const scene, const ZF4EntID entID, const ZF4GamePtrs* const gamePtrs) {
    ZF4Ent* const ent = zf4_get_ent(&scene->entManager, entID);
    HitboxEntExt* const entExt = zf4_get_ent_type_ext(&scene->entManager, entID);

    ZF4EntID enemyEntCollisionIDs[32] = {0};
    const int enemyEntCollisionCnt = zf4_get_colliding_ents(enemyEntCollisionIDs, ZF4_STATIC_ARRAY_LEN(enemyEntCollisionIDs), enemy_ent_collision_filter, entID, &scene->entManager);

    for (int i = 0; i < enemyEntCollisionCnt; ++i) {
        hurt_enemy_ent(&scene->entManager, enemyEntCollisionIDs[i], entExt->dmg, entExt->force);
    }

    zf4_write_ent_render_data(&scene->renderer, ent, 0);

    zf4_destroy_ent(&scene->entManager, entID);

    return true;
}
