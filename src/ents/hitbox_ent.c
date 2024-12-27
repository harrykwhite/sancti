#include "ent_types.h"

#include "../sprites.h"

bool init_hitbox_ent(ZF4Scene* scene, ZF4EntID entID, ZF4GamePtrs* gamePtrs) {
    ZF4Ent* ent = zf4_get_ent(entID, &scene->entManager);
    ent->spriteIndex = BLUE_PIXEL_SPRITE;

    return true;
}

bool hitbox_ent_tick(ZF4Scene* scene, ZF4EntID entID, ZF4GamePtrs* gamePtrs) {
    ZF4Ent* ent = zf4_get_ent(entID, &scene->entManager);
    HitboxEntExt* entExt = zf4_get_ent_type_ext(entID, &scene->entManager);

    ZF4EntID enemyEntCollisionIDs[32] = {0};
    int enemyEntCollisionCnt = zf4_get_colliding_ents(entID, enemyEntCollisionIDs, ZF4_STATIC_ARRAY_LEN(enemyEntCollisionIDs), enemy_ent_filter, &scene->entManager);

    for (int i = 0; i < enemyEntCollisionCnt; ++i) {
        hurt_enemy_ent(enemyEntCollisionIDs[i], &scene->entManager, entExt->dmg, entExt->force);
    }

    zf4_write_ent_render_data(ent, &scene->renderer, 0, 1.0f);

    zf4_destroy_ent(entID, &scene->entManager);

    return true;
}
