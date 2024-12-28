#ifndef WORLD_H
#define WORLD_H

#include <zf4.h>

typedef enum {
    CAM_WORLD_RENDER_LAYER,
    UI_WORLD_RENDER_LAYER,

    WORLD_RENDER_LAYER_CNT
} WorldRenderLayer;

typedef struct {
    ZF4EntID playerEntID;
    int playerEntHPLast;
    ZF4CharBatchID playerEntHPTextCBID;
} World;

void init_world_render_layer_props(ZF4RenderLayerProps* props, int layerIndex);
int load_world_screen_ent_type_ext_limit(int typeIndex);
bool init_world(ZF4Scene* scene, ZF4GamePtrs* gamePtrs);
bool world_tick(ZF4Scene* scene, int* sceneChangeIndex, ZF4GamePtrs* gamePtrs);

#endif
