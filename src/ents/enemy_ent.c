#include "ent_types.h"

#include "../sprites.h"

bool init_enemy_ent(ZF4Scene* scene, ZF4EntID entID, ZF4GamePtrs* gamePtrs) {
    ZF4Ent* enemy = zf4_get_ent(entID, &scene->entManager);
    EnemyEntExt* enemyExt = zf4_get_ent_type_ext(entID, &scene->entManager);

    enemy->spriteIndex = ENEMY_SPRITE;
    enemyExt->hp = 25;
    enemyExt->contactDmg = 10;
    enemyExt->contactStrength = 11.0f;

    return true;
}

bool enemy_ent_tick(ZF4Scene* scene, ZF4EntID entID, ZF4GamePtrs* gamePtrs) {
    ZF4Ent* enemy = zf4_get_ent(entID, &scene->entManager);
    EnemyEntExt* enemyExt = zf4_get_ent_type_ext(entID, &scene->entManager);

    enemyExt->vel = zf4_lerp_vec_2d(enemyExt->vel, (ZF4Vec2D) { 0 }, 0.25f);
    enemy->pos = zf4_calc_vec_2d_sum(enemy->pos, enemyExt->vel);

    zf4_write_ent_render_data(enemy, &scene->renderer, 0, 1.0f);

    return true;
}

void hurt_enemy_ent(ZF4EntID entID, ZF4EntManager* entManager, int dmg, ZF4Vec2D force) {
    EnemyEntExt* entExt = zf4_get_ent_type_ext(entID, entManager);

    entExt->hp -= dmg;
    entExt->vel = zf4_calc_vec_2d_sum(entExt->vel, force);

    if (entExt->hp <= 0) {
        zf4_destroy_ent(entID, entManager);
    }
}
