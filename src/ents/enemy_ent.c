#include "ent_types.h"

#include "../sprites.h"

bool init_enemy_ent(ZF4Scene* scene, ZF4EntID entID, ZF4GamePtrs* gamePtrs) {
    ZF4Ent* ent = zf4_get_ent(&scene->entManager, entID);
    EnemyEntExt* entExt = zf4_get_ent_type_ext(&scene->entManager, entID);

    ent->spriteIndex = ENEMY_SPRITE;
    entExt->hp = 5;

    return true;
}

bool enemy_ent_tick(ZF4Scene* scene, ZF4EntID entID, ZF4GamePtrs* gamePtrs) {
    ZF4Ent* ent = zf4_get_ent(&scene->entManager, entID);
    EnemyEntExt* entExt = zf4_get_ent_type_ext(&scene->entManager, entID);

    entExt->vel = zf4_lerp_vec_2d(entExt->vel, (ZF4Vec2D) { 0 }, 0.25f);
    ent->pos = zf4_calc_vec_2d_sum(ent->pos, entExt->vel);

    zf4_write_ent_render_data(&scene->renderer, ent, 0);

    return true;
}

void hurt_enemy_ent(ZF4EntManager* entManager, ZF4EntID entID, int dmg, ZF4Vec2D force) {
    EnemyEntExt* entExt = zf4_get_ent_type_ext(entManager, entID);

    entExt->hp -= dmg;
    entExt->vel = zf4_calc_vec_2d_sum(entExt->vel, force);

    if (entExt->hp <= 0) {
        zf4_destroy_ent(entManager, entID);
    }
}
