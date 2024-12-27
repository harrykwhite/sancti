#ifndef TITLE_SCREEN_H
#define TITLE_SCREEN_H

#include <zf4.h>

typedef enum {
    GENERAL_TITLE_SCREEN_RENDER_LAYER,

    TITLE_SCREEN_RENDER_LAYER_CNT
} TitleScreenRenderLayer;

typedef struct {
    ZF4CharBatchID titleTextCBID;
    ZF4CharBatchID startTextCBID;
} TitleScreen;

void init_title_screen_render_layer_props(ZF4RenderLayerProps* props, int layerIndex);
bool init_title_screen(ZF4Scene* scene, ZF4GamePtrs* gamePtrs);
bool title_screen_tick(ZF4Scene* scene, int* sceneChangeIndex, ZF4GamePtrs* gamePtrs);

#endif
