#include "world.h"

#include "../asset_indexes.h"

#define WORLD_SIZE ((ZF4Vec2D) {2560, 1440})

#define CAM_SCALE 2.0f
#define CAM_LOOK_DIST_LIMIT 24.0f
#define CAM_LOOK_DIST_SCALAR_DIST (CAM_LOOK_DIST_LIMIT * 32.0f)
#define CAM_POS_LERP 0.25f

#define BG_COLOR ((ZF4Vec3D) {0.59f, 0.79f, 0.93f})

void init_world_render_layer_props(ZF4RenderLayerProps* const props, const int layerIndex) {
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

int load_world_screen_ent_type_ext_limit(const int typeIndex) {
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

bool init_world(ZF4Scene* const scene, const ZF4GamePtrs* const gamePtrs) {
    World* const world = scene->userData;

    scene->renderer.cam.scale = CAM_SCALE;
    scene->renderer.bgColor = BG_COLOR;

    world->player = zf4_spawn_ent(scene, PLAYER_ENT, zf4_calc_vec_2d_scaled(WORLD_SIZE, 0.5f), gamePtrs);
    scene->renderer.cam.pos = zf4_get_ent(&scene->entManager, world->player)->pos;

    const ZF4Vec2D camSize = zf4_get_camera_size(&scene->renderer.cam);

    for (int i = 0; i < 10; ++i) {
        const ZF4Vec2D enemyPos = (ZF4Vec2D) {
            zf4_gen_rand_float(0.0f, WORLD_SIZE.x),
            zf4_gen_rand_float(0.0f, WORLD_SIZE.y)
        };

        zf4_spawn_ent(scene, ENEMY_ENT, enemyPos, gamePtrs);
    }

    return true;
}

bool world_tick(ZF4Scene* const scene, int* const sceneChangeIndex, const ZF4GamePtrs* const gamePtrs) {
    World* const world = scene->userData;

    //
    // Camera
    //
    {
        ZF4Camera* const cam = &scene->renderer.cam;
        const ZF4Ent* const playerEnt = zf4_get_ent(&scene->entManager, world->player);

        // Determine the target position.
        const ZF4Vec2D mouseCamPos = zf4_screen_to_camera_pos(zf4_get_mouse_pos(), &scene->renderer.cam);
        const ZF4Vec2D playerToMouseCamPosDiff = zf4_calc_vec_2d_diff(mouseCamPos, playerEnt->pos);
        const float playerToMouseCamPosDist = zf4_calc_vec_2d_mag(playerToMouseCamPosDiff);
        const ZF4Vec2D playerToMouseCamPosDir = zf4_calc_vec_2d_normal(playerToMouseCamPosDiff);

        const float lookDist = CAM_LOOK_DIST_LIMIT * ZF4_MIN(playerToMouseCamPosDist / CAM_LOOK_DIST_SCALAR_DIST, 1.0f);
        const ZF4Vec2D lookOffs = zf4_calc_vec_2d_scaled(playerToMouseCamPosDir, lookDist);

        const ZF4Vec2D targPos = zf4_calc_vec_2d_sum(playerEnt->pos, lookOffs);

        // Approach the target position.
        cam->pos.x = zf4_lerp(cam->pos.x, targPos.x, CAM_POS_LERP);
        cam->pos.y = zf4_lerp(cam->pos.y, targPos.y, CAM_POS_LERP);

        // Clamp the camera position within world boundaries.
        const ZF4Vec2D camSize = zf4_get_camera_size(cam);
        cam->pos.x = ZF4_CLAMP(cam->pos.x, camSize.x / 2.0f, WORLD_SIZE.x - (camSize.x / 2.0f));
        cam->pos.y = ZF4_CLAMP(cam->pos.y, camSize.y / 2.0f, WORLD_SIZE.y - (camSize.y / 2.0f));
    }

    //
    // Cursor
    //
    {
        const ZF4SpriteBatchWriteInfo sbWriteInfo = {
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
