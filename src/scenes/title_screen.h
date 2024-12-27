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

void init_title_screen_render_layer_props(ZF4RenderLayerProps* const props, const int layerIndex);
bool init_title_screen(ZF4Scene* const scene, const ZF4GamePtrs* const gamePtrs);
bool title_screen_tick(ZF4Scene* const scene, int* const sceneChangeIndex, const ZF4GamePtrs* const gamePtrs);

#endif
