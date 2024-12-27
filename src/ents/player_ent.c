#include "ent_types.h"

#include "../sprites.h"

#define SWORD_CHARGE_TIME_MAX 15
#define SWORD_CHARGE_ROT_OFFS (ZF4_PI * 0.15f)
#define SWORD_HITBOX_SIZE 24.0f
#define SWORD_HITBOX_OFFS_DIST 32.0f
#define SWORD_HITBOX_DMG 1
#define SWORD_HITBOX_STRENGTH 11.0f
#define SWORD_ROT_OFFS_MAX (ZF4_PI * 0.7f)
#define SWORD_OFFS_DIST 6.0f
#define SWORD_ORIGIN (ZF4Vec2D) {0.0f, 0.5f}

static float calc_move_spd(const PlayerEntExt* const playerEntExt) {
    const float moveSpd = 3.0f;
    const float swordChargeMult = 0.4f;
    const float mult = swordChargeMult + ((1.0f - ((float)playerEntExt->swordChargeTime / SWORD_CHARGE_TIME_MAX)) * (1.0f - swordChargeMult));
    return moveSpd * mult;
}

bool init_player_ent(ZF4Scene* const scene, const ZF4EntID entID, const ZF4GamePtrs* const gamePtrs) {
    ZF4Ent* const playerEnt = zf4_get_ent(&scene->entManager, entID);
    playerEnt->spriteIndex = PLAYER_SPRITE;

    return true;
}

bool player_ent_tick(ZF4Scene* const scene, const ZF4EntID entID, const ZF4GamePtrs* const gamePtrs) {
    ZF4Ent* const playerEnt = zf4_get_ent(&scene->entManager, entID);
    PlayerEntExt* const playerEntExt = zf4_get_ent_type_ext(&scene->entManager, entID);

    //
    // Sword Charging
    //
    if (zf4_is_mouse_button_down(ZF4_MOUSE_BUTTON_LEFT)) {
        if (playerEntExt->swordChargeTime < SWORD_CHARGE_TIME_MAX) {
            ++playerEntExt->swordChargeTime;
        }
    }

    //
    // Movement
    //
    const ZF4Vec2D moveAxis = {
        zf4_is_key_down(ZF4_KEY_D) - zf4_is_key_down(ZF4_KEY_A),
        zf4_is_key_down(ZF4_KEY_S) - zf4_is_key_down(ZF4_KEY_W)
    };
    
    const ZF4Vec2D velTarg = zf4_calc_vec_2d_scaled(moveAxis, calc_move_spd(playerEntExt));
    const float velLerp = 0.25f;

    playerEntExt->vel.x = zf4_lerp(playerEntExt->vel.x, velTarg.x, velLerp);
    playerEntExt->vel.y = zf4_lerp(playerEntExt->vel.y, velTarg.y, velLerp);

    playerEnt->pos = zf4_calc_vec_2d_sum(playerEnt->pos, playerEntExt->vel);

    //
    // Sword Attacking
    //
    const ZF4Vec2D mouseCamPos = zf4_screen_to_camera_pos(zf4_get_mouse_pos(), &scene->renderer.cam);

    if (zf4_is_mouse_button_released(ZF4_MOUSE_BUTTON_LEFT)) {
        playerEntExt->swordChargeTime = 0;
        playerEntExt->swordRotAxis = !playerEntExt->swordRotAxis;

        const ZF4Vec2D hitboxDir = zf4_calc_vec_2d_dir(playerEnt->pos, mouseCamPos);
        const ZF4Vec2D hitboxOffs = zf4_calc_vec_2d_scaled(hitboxDir, SWORD_HITBOX_OFFS_DIST);
        const ZF4Vec2D hitboxPos = zf4_calc_vec_2d_sum(playerEnt->pos, hitboxOffs);
        const ZF4Vec2D hitboxForce = zf4_calc_vec_2d_scaled(hitboxDir, SWORD_HITBOX_STRENGTH);

        const ZF4EntID hitboxEntID = zf4_spawn_ent(scene, HITBOX_ENT, hitboxPos, gamePtrs);
        
        ZF4Ent* const hitboxEnt = zf4_get_ent(&scene->entManager, hitboxEntID);
        hitboxEnt->scale = (ZF4Vec2D) {SWORD_HITBOX_SIZE, SWORD_HITBOX_SIZE};

        HitboxEntExt* const hitboxEntExt = zf4_get_ent_type_ext(&scene->entManager, hitboxEntID);
        hitboxEntExt->dmg = SWORD_HITBOX_DMG;
        hitboxEntExt->force = hitboxForce;
        hitboxEntExt->collider = (ZF4RectF) {
            hitboxPos.x - (SWORD_HITBOX_SIZE / 2.0f),
            hitboxPos.y - (SWORD_HITBOX_SIZE / 2.0f),
            SWORD_HITBOX_SIZE,
            SWORD_HITBOX_SIZE
        };
    }

    const float swordRotOffsTargAbs = SWORD_ROT_OFFS_MAX + (((float)playerEntExt->swordChargeTime / SWORD_CHARGE_TIME_MAX) * SWORD_CHARGE_ROT_OFFS);
    const float swordRotOffsTarg = ((playerEntExt->swordRotAxis ? 1 : -1) * swordRotOffsTargAbs);
    playerEntExt->swordRotOffs = zf4_lerp(playerEntExt->swordRotOffs, swordRotOffsTarg, 0.4f);

    playerEntExt->swordRot = zf4_calc_vec_2d_dir_rads(playerEnt->pos, mouseCamPos) + playerEntExt->swordRotOffs;

    // PROBLEM: The render layer is scene-specific, but the entity is not! Perhaps this is a sign from the gods indicating that I should remove the layer system?
    zf4_write_ent_render_data(&scene->renderer, playerEnt, 0);

    //
    const ZF4Vec2D swordOffs = zf4_calc_len_dir_vec_2d(SWORD_OFFS_DIST, playerEntExt->swordRot);
    const ZF4Vec2D swordPos = zf4_calc_vec_2d_sum(playerEnt->pos, swordOffs);

    const ZF4SpriteBatchWriteInfo swordSBWriteInfo = {
        .texIndex = zf4_get_sprite(SWORD_SPRITE)->texIndex,
        .pos = swordPos,
        .srcRect = zf4_get_sprite_src_rect(SWORD_SPRITE, 0),
        .origin = SWORD_ORIGIN,
        .rot = playerEntExt->swordRot,
        .scale = {1.0f, 1.0f},
        .alpha = 1.0f
    };

    zf4_write_to_sprite_batch(&scene->renderer, 0, &swordSBWriteInfo);

    return true;
}
