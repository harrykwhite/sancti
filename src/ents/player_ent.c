#include "ent_types.h"

#include "../sprites.h"

#define HP_LIMIT 100
#define INV_TIME_MAX 30
#define INV_ALPHA_MIN 0.5f
#define INV_ALPHA_MAX 0.7f

#define SWORD_CHARGE_TIME_MAX 15
#define SWORD_CHARGE_ROT_OFFS (ZF4_PI * 0.15f)
#define SWORD_HITBOX_SIZE 24.0f
#define SWORD_HITBOX_OFFS_DIST 32.0f
#define SWORD_HITBOX_DMG 1
#define SWORD_HITBOX_STRENGTH 11.0f
#define SWORD_ROT_OFFS_MAX (ZF4_PI * 0.7f)
#define SWORD_OFFS_DIST 6.0f
#define SWORD_ORIGIN (ZF4Vec2D) {0.0f, 0.5f}

static float calc_move_spd(PlayerEntExt* playerEntExt) {
    float moveSpd = 3.0f;
    float swordChargeMult = 0.4f;
    float mult = swordChargeMult + ((1.0f - ((float)playerEntExt->swordChargeTime / SWORD_CHARGE_TIME_MAX)) * (1.0f - swordChargeMult));
    return moveSpd * mult;
}

static float calc_alpha(PlayerEntExt* playerEntExt) {
    float alpha = 1.0f;

    if (playerEntExt->invTime > 0) {
        alpha = playerEntExt->invTime & 1 ? INV_ALPHA_MAX : INV_ALPHA_MIN;
    }

    return alpha;
}

bool init_player_ent(ZF4Scene* scene, ZF4EntID playerID, ZF4GamePtrs* gamePtrs) {
    ZF4Ent* player = zf4_get_ent(playerID, &scene->entManager);
    PlayerEntExt* playerExt = zf4_get_ent_type_ext(playerID, &scene->entManager);

    player->spriteIndex = PLAYER_SPRITE;
    playerExt->hp = HP_LIMIT;

    return true;
}

bool player_ent_tick(ZF4Scene* scene, ZF4EntID playerID, ZF4GamePtrs* gamePtrs) {
    ZF4Ent* player = zf4_get_ent(playerID, &scene->entManager);
    PlayerEntExt* playerExt = zf4_get_ent_type_ext(playerID, &scene->entManager);

    ZF4Vec2D mouseCamPos = zf4_screen_to_camera_pos(zf4_get_mouse_pos(), &scene->renderer.cam);

    //
    // Sword Charging
    //
    if (zf4_is_mouse_button_down(ZF4_MOUSE_BUTTON_LEFT)) {
        if (playerExt->swordChargeTime < SWORD_CHARGE_TIME_MAX) {
            ++playerExt->swordChargeTime;
        }
    }

    //
    // Movement
    //
    ZF4Vec2D moveAxis = {
        zf4_is_key_down(ZF4_KEY_D) - zf4_is_key_down(ZF4_KEY_A),
        zf4_is_key_down(ZF4_KEY_S) - zf4_is_key_down(ZF4_KEY_W)
    };

    ZF4Vec2D velTarg = zf4_calc_vec_2d_scaled(moveAxis, calc_move_spd(playerExt));
    float velLerp = 0.25f;

    playerExt->vel.x = zf4_lerp(playerExt->vel.x, velTarg.x, velLerp);
    playerExt->vel.y = zf4_lerp(playerExt->vel.y, velTarg.y, velLerp);

    player->pos = zf4_calc_vec_2d_sum(player->pos, playerExt->vel);

    //
    // Enemy Collisions
    //
    {
        ZF4EntID collidingEnemyID; // Invincibility will cancel out subsequent collision hits, so we only need to do one.

        if (zf4_get_colliding_ents(playerID, &collidingEnemyID, 1, enemy_ent_filter, &scene->entManager)) {
            ZF4Ent* enemy = zf4_get_ent(collidingEnemyID, &scene->entManager);
            EnemyEntExt* enemyExt = zf4_get_ent_type_ext(collidingEnemyID, &scene->entManager);

            ZF4Vec2D enemyToPlayerDir = zf4_calc_vec_2d_dir(enemy->pos, player->pos);
            ZF4Vec2D force = zf4_calc_vec_2d_scaled(enemyToPlayerDir, enemyExt->contactStrength);

            hurt_player_ent(playerID, &scene->entManager, enemyExt->contactDmg, force);
        }
    }

    //
    // Sword Attacking
    //
    if (zf4_is_mouse_button_released(ZF4_MOUSE_BUTTON_LEFT)) {
        playerExt->swordChargeTime = 0;
        playerExt->swordRotAxis = !playerExt->swordRotAxis;

        ZF4Vec2D hitboxDir = zf4_calc_vec_2d_dir(player->pos, mouseCamPos);
        ZF4Vec2D hitboxOffs = zf4_calc_vec_2d_scaled(hitboxDir, SWORD_HITBOX_OFFS_DIST);
        ZF4Vec2D hitboxPos = zf4_calc_vec_2d_sum(player->pos, hitboxOffs);
        ZF4Vec2D hitboxForce = zf4_calc_vec_2d_scaled(hitboxDir, SWORD_HITBOX_STRENGTH);

        ZF4EntID hitboxEntID = zf4_spawn_ent(HITBOX_ENT, hitboxPos, scene, gamePtrs);

        ZF4Ent* hitboxEnt = zf4_get_ent(hitboxEntID, &scene->entManager);
        hitboxEnt->scale = (ZF4Vec2D) {SWORD_HITBOX_SIZE, SWORD_HITBOX_SIZE};

        HitboxEntExt* hitboxEntExt = zf4_get_ent_type_ext(hitboxEntID, &scene->entManager);
        hitboxEntExt->dmg = SWORD_HITBOX_DMG;
        hitboxEntExt->force = hitboxForce;
        hitboxEntExt->collider = (ZF4RectF) {
            hitboxPos.x - (SWORD_HITBOX_SIZE / 2.0f),
            hitboxPos.y - (SWORD_HITBOX_SIZE / 2.0f),
            SWORD_HITBOX_SIZE,
            SWORD_HITBOX_SIZE
        };
    }

    float swordRotOffsTargAbs = SWORD_ROT_OFFS_MAX + (((float)playerExt->swordChargeTime / SWORD_CHARGE_TIME_MAX) * SWORD_CHARGE_ROT_OFFS);
    float swordRotOffsTarg = ((playerExt->swordRotAxis ? 1 : -1) * swordRotOffsTargAbs);
    playerExt->swordRotOffs = zf4_lerp(playerExt->swordRotOffs, swordRotOffsTarg, 0.4f);

    playerExt->swordRot = zf4_calc_vec_2d_dir_rads(player->pos, mouseCamPos) + playerExt->swordRotOffs;

    // PROBLEM: The render layer is scene-specific, but the entity is not! Perhaps this is a sign from the gods indicating that I should remove the layer system?
    zf4_write_ent_render_data(player, &scene->renderer, 0, calc_alpha(playerExt));

    ZF4Vec2D swordOffs = zf4_calc_len_dir_vec_2d(SWORD_OFFS_DIST, playerExt->swordRot);
    ZF4Vec2D swordPos = zf4_calc_vec_2d_sum(player->pos, swordOffs);

    ZF4SpriteBatchWriteInfo swordSBWriteInfo = {
        .texIndex = zf4_get_sprite(SWORD_SPRITE)->texIndex,
        .pos = swordPos,
        .srcRect = zf4_get_sprite_src_rect(SWORD_SPRITE, 0),
        .origin = SWORD_ORIGIN,
        .rot = playerExt->swordRot,
        .scale = {1.0f, 1.0f},
        .alpha = 1.0f
    };

    zf4_write_to_sprite_batch(&scene->renderer, 0, &swordSBWriteInfo);

    //
    // Invincibility
    //
    if (playerExt->invTime > 0) {
        --playerExt->invTime;
    }

    return true;
}

void hurt_player_ent(ZF4EntID entID, ZF4EntManager* entManager, int dmg, ZF4Vec2D force) {
    assert(zf4_get_ent(entID, entManager)->typeIndex == PLAYER_ENT);

    PlayerEntExt* playerExt = zf4_get_ent_type_ext(entID, entManager);

    if (playerExt->invTime > 0) {
        return;
    }

    playerExt->hp -= dmg;
    playerExt->invTime = INV_TIME_MAX;
    playerExt->vel = zf4_calc_vec_2d_sum(playerExt->vel, force);

    if (playerExt->hp <= 0) {
        zf4_destroy_ent(entID, entManager);
    }
}
