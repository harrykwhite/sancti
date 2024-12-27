#include "ent_types.h"

#include "../sprites.h"

bool init_enemy_ent(ZF4Scene* const scene, const ZF4EntID entID, const ZF4GamePtrs* const gamePtrs) {
    ZF4Ent* const ent = zf4_get_ent(&scene->entManager, entID);
    EnemyEntExt* const entExt = zf4_get_ent_type_ext(&scene->entManager, entID);

    ent->spriteIndex = ENEMY_SPRITE;
    entExt->hp = 5;

    return true;
}

bool enemy_ent_tick(ZF4Scene* const scene, const ZF4EntID entID, const ZF4GamePtrs* const gamePtrs) {
    ZF4Ent* const ent = zf4_get_ent(&scene->entManager, entID);
    EnemyEntExt* const entExt = zf4_get_ent_type_ext(&scene->entManager, entID);

    entExt->vel = zf4_lerp_vec_2d(entExt->vel, (ZF4Vec2D) { 0 }, 0.25f);
    ent->pos = zf4_calc_vec_2d_sum(ent->pos, entExt->vel);

    zf4_write_ent_render_data(&scene->renderer, ent, 0);

    return true;
}

void hurt_enemy_ent(ZF4EntManager* const entManager, const ZF4EntID entID, const int dmg, const ZF4Vec2D force) {
    EnemyEntExt* const entExt = zf4_get_ent_type_ext(entManager, entID);

    entExt->hp -= dmg;
    entExt->vel = zf4_calc_vec_2d_sum(entExt->vel, force);

    if (entExt->hp <= 0) {
        zf4_destroy_ent(entManager, entID);
    }
}
