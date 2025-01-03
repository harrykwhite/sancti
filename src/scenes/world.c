#include "world.h"

#include "../asset_indexes.h"
#include "../sprites.h"
#include "../ents/ent_types.h"

#define WORLD_SIZE ((ZF4Vec2D) {2560, 1440})

#define CAM_SCALE 2.0f
#define CAM_LOOK_DIST_LIMIT 24.0f
#define CAM_LOOK_DIST_SCALAR_DIST (CAM_LOOK_DIST_LIMIT * 32.0f)
#define CAM_POS_LERP 0.25f

#define BG_COLOR ((ZF4Vec3D) {0.59f, 0.79f, 0.93f})

#define HP_TEXT_LEN_LIMIT 11

static void write_ent_hp_text_to_char_batch(ZF4CharBatchID cbID, ZF4Renderer* renderer, int hp, int hpLimit) {
    char text[HP_TEXT_LEN_LIMIT + 1];
    snprintf(text, sizeof(text), "%d/%d", hp, hpLimit);
    zf4_write_to_char_batch(renderer, cbID, text, ZF4_FONT_HOR_ALIGN_CENTER, ZF4_FONT_VER_ALIGN_CENTER);
}

static ZF4Vec2D calc_ent_hp_text_pos(ZF4Ent* player, ZF4Camera* cam) {
    ZF4RectF playerCollider = zf4_get_ent_collider(player);
    const ZF4Vec2D textCamPos = {
        player->pos.x,
        zf4_get_rect_f_bottom(&playerCollider) + 10.0f
    };

    return zf4_camera_to_screen_pos(textCamPos, cam);
}

void init_world_render_layer_props(ZF4RenderLayerProps* props, int layerIndex) {
    switch (layerIndex) {
        case CAM_WORLD_RENDER_LAYER:
            props->spriteBatchCnt = 1;
            props->charBatchCnt = 0;
            break;

        case UI_WORLD_RENDER_LAYER:
            props->spriteBatchCnt = 1;
            props->charBatchCnt = 1;
            break;
    }
}

int load_world_screen_ent_type_ext_limit(int typeIndex) {
    switch (typeIndex) {
        case PLAYER_ENT:
            return 1;

        case ENEMY_ENT:
            return 32;

        case HITBOX_ENT:
            return 32;

        default:
            return 0;
    }
}

bool init_world(ZF4Scene* scene, ZF4GamePtrs* gamePtrs) {
    World* world = scene->userData;

    scene->renderer.cam.scale = CAM_SCALE;
    scene->renderer.bgColor = BG_COLOR;

    world->playerEntID = zf4_spawn_ent(PLAYER_ENT, zf4_calc_vec_2d_scaled(WORLD_SIZE, 0.5f), scene, gamePtrs);
    scene->renderer.cam.pos = zf4_get_ent(world->playerEntID, &scene->entManager)->pos;

    world->playerEntHPTextCBID = zf4_activate_any_char_batch(&scene->renderer, UI_WORLD_RENDER_LAYER, HP_TEXT_LEN_LIMIT, EB_GARAMOND_18_FONT);
    PlayerEntExt* playerEntExt = zf4_get_ent_type_ext(world->playerEntID, &scene->entManager);
    write_ent_hp_text_to_char_batch(world->playerEntHPTextCBID, &scene->renderer, playerEntExt->hp, PLAYER_HP_LIMIT);

    ZF4Vec2D camSize = zf4_get_camera_size(&scene->renderer.cam);

    for (int i = 0; i < 10; ++i) {
        ZF4Vec2D enemyPos = (ZF4Vec2D) {
            zf4_gen_rand_float(0.0f, WORLD_SIZE.x),
            zf4_gen_rand_float(0.0f, WORLD_SIZE.y)
        };

        zf4_spawn_ent(ENEMY_ENT, enemyPos, scene, gamePtrs);
    }

    return true;
}

bool world_tick(ZF4Scene* scene, int* sceneChangeIndex, ZF4GamePtrs* gamePtrs) {
    World* world = scene->userData;

    //
    // Camera
    //
    {
        ZF4Camera* cam = &scene->renderer.cam;

        // Determine the target position.
        ZF4Vec2D targPos = cam->pos;

        if (zf4_does_ent_exist(world->playerEntID, &scene->entManager)) {
            ZF4Ent* playerEnt = zf4_get_ent(world->playerEntID, &scene->entManager);

            ZF4Vec2D mouseCamPos = zf4_screen_to_camera_pos(zf4_get_mouse_pos(), &scene->renderer.cam);
            ZF4Vec2D playerToMouseCamPosDiff = zf4_calc_vec_2d_diff(mouseCamPos, playerEnt->pos);
            float playerToMouseCamPosDist = zf4_calc_vec_2d_mag(playerToMouseCamPosDiff);
            ZF4Vec2D playerToMouseCamPosDir = zf4_calc_vec_2d_normal(playerToMouseCamPosDiff);

            float lookDist = CAM_LOOK_DIST_LIMIT * ZF4_MIN(playerToMouseCamPosDist / CAM_LOOK_DIST_SCALAR_DIST, 1.0f);
            ZF4Vec2D lookOffs = zf4_calc_vec_2d_scaled(playerToMouseCamPosDir, lookDist);

            targPos = zf4_calc_vec_2d_sum(playerEnt->pos, lookOffs);
        }

        // Approach the target position.
        cam->pos.x = zf4_lerp(cam->pos.x, targPos.x, CAM_POS_LERP);
        cam->pos.y = zf4_lerp(cam->pos.y, targPos.y, CAM_POS_LERP);

        // Clamp the camera position within world boundaries.
        ZF4Vec2D camSize = zf4_get_camera_size(cam);
        cam->pos.x = ZF4_CLAMP(cam->pos.x, camSize.x / 2.0f, WORLD_SIZE.x - (camSize.x / 2.0f));
        cam->pos.y = ZF4_CLAMP(cam->pos.y, camSize.y / 2.0f, WORLD_SIZE.y - (camSize.y / 2.0f));
    }

    //
    // Player Entity HP Text
    //
    if (zf4_does_ent_exist(world->playerEntID, &scene->entManager)) {
        ZF4Ent* playerEnt = zf4_get_ent(world->playerEntID, &scene->entManager);
        PlayerEntExt* playerEntExt = zf4_get_ent_type_ext(world->playerEntID, &scene->entManager);

        if (playerEntExt->hp != world->playerEntHPLast) {
            write_ent_hp_text_to_char_batch(world->playerEntHPTextCBID, &scene->renderer, playerEntExt->hp, PLAYER_HP_LIMIT);
        }

        const ZF4Vec2D textPos = calc_ent_hp_text_pos(playerEnt, &scene->renderer.cam);
        zf4_get_char_batch_display_props(&scene->renderer, world->playerEntHPTextCBID)->pos = textPos;

        world->playerEntHPLast = playerEntExt->hp; // NOTE: Move down?
    } else {
        // NOTE: Might need to change how this is handled, as it's a bit hacky.
        if (world->playerEntHPLast != 0) {
            zf4_clear_char_batch(&scene->renderer, world->playerEntHPTextCBID);
            world->playerEntHPLast = 0;
        }
    }

    //
    // Cursor
    //
    {
        ZF4SpriteBatchWriteInfo sbWriteInfo = {
            .texIndex = zf4_get_sprite(CURSOR_SPRITE)->texIndex,
            .pos = zf4_get_mouse_pos(),
            .srcRect = zf4_get_sprite_src_rect(CURSOR_SPRITE, 0),
            .origin = {0.5f, 0.5f},
            .rot = 0.0f,
            .scale = {scene->renderer.cam.scale, scene->renderer.cam.scale},
            .alpha = 1.0f
        };

        zf4_write_to_sprite_batch(&scene->renderer, UI_WORLD_RENDER_LAYER, &sbWriteInfo);
    }

    return true;
}
