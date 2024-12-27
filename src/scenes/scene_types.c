#include "scene_types.h"

#include <stdalign.h>
#include "title_screen.h"
#include "world.h"

void load_scene_type_info(ZF4SceneTypeInfo* const typeInfo, const int typeIndex) {
    switch (typeIndex) {
        case TITLE_SCREEN_SCENE:
            typeInfo->memArenaSize = ZF4_MEGABYTES(1);
            typeInfo->renderLayerCnt = TITLE_SCREEN_RENDER_LAYER_CNT;
            typeInfo->camRenderLayerCnt = 0;
            typeInfo->renderLayerPropsInitializer = init_title_screen_render_layer_props;
            typeInfo->init = init_title_screen;
            typeInfo->tick = title_screen_tick;
            typeInfo->userDataSize = sizeof(TitleScreen);
            typeInfo->userDataAlignment = alignof(TitleScreen);
            break;

        case WORLD_SCENE:
            typeInfo->memArenaSize = ZF4_MEGABYTES(4);
            typeInfo->renderLayerCnt = WORLD_RENDER_LAYER_CNT;
            typeInfo->camRenderLayerCnt = UI_WORLD_RENDER_LAYER;
            typeInfo->renderLayerPropsInitializer = init_world_render_layer_props;
            typeInfo->entLimit = 32;
            typeInfo->entTypeExtLimitLoader = load_world_screen_ent_type_ext_limit;
            typeInfo->init = init_world;
            typeInfo->tick = world_tick;
            typeInfo->userDataSize = sizeof(World);
            typeInfo->userDataAlignment = alignof(World);
            break;
    }
}
