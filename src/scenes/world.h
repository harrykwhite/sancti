#ifndef WORLD_H
#define WORLD_H

#include <zf4.h>
#include "../sprites.h"
#include "../ents/ent_types.h"

typedef enum {
    CAM_WORLD_RENDER_LAYER,
    UI_WORLD_RENDER_LAYER,

    WORLD_RENDER_LAYER_CNT
} WorldRenderLayer;

typedef struct {
    ZF4EntID player;
} World;

void init_world_render_layer_props(ZF4RenderLayerProps* const props, const int layerIndex);
int load_world_screen_ent_type_ext_limit(const int typeIndex);
bool init_world(ZF4Scene* const scene, const ZF4GamePtrs* const gamePtrs);
bool world_tick(ZF4Scene* const scene, int* const sceneChangeIndex, const ZF4GamePtrs* const gamePtrs);

#endif
