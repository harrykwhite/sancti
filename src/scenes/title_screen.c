#include "title_screen.h"

#include <stdalign.h>
#include "../asset_indexes.h"
#include "../ents/ent_types.h"
#include "scene_types.h"

static void refresh_text_positions(ZF4Renderer* renderer, TitleScreen* titleScreen) {
    ZF4CharBatchDisplayProps* titleTextDisplayProps = zf4_get_char_batch_display_props(renderer, titleScreen->titleTextCBID);
    titleTextDisplayProps->pos = (ZF4Vec2D) {
        zf4_get_window_size().x / 2.0f,
        (zf4_get_window_size().y / 2.0f) - 40.0f
    };

    ZF4CharBatchDisplayProps* startTextDisplayProps = zf4_get_char_batch_display_props(renderer, titleScreen->startTextCBID);
    startTextDisplayProps->pos = (ZF4Vec2D) {
        zf4_get_window_size().x / 2.0f,
        (zf4_get_window_size().y / 2.0f) + 32.0f
    };
}

void init_title_screen_render_layer_props(ZF4RenderLayerProps* props, int layerIndex) {
    switch (layerIndex) {
        case GENERAL_TITLE_SCREEN_RENDER_LAYER:
            props->spriteBatchCnt = 0;
            props->charBatchCnt = 2;
            break;
    }
}

bool init_title_screen(ZF4Scene* scene, ZF4GamePtrs* gamePtrs) {
    TitleScreen* ts = scene->userData;

    ts->titleTextCBID = zf4_activate_any_char_batch(&scene->renderer, GENERAL_TITLE_SCREEN_RENDER_LAYER, 8, EB_GARAMOND_72_FONT);
    zf4_write_to_char_batch(&scene->renderer, ts->titleTextCBID, "Sancti", ZF4_FONT_HOR_ALIGN_CENTER, ZF4_FONT_VER_ALIGN_CENTER);

    ts->startTextCBID = zf4_activate_any_char_batch(&scene->renderer, GENERAL_TITLE_SCREEN_RENDER_LAYER, 32, EB_GARAMOND_18_FONT);
    zf4_write_to_char_batch(&scene->renderer, ts->startTextCBID, "Press Enter to Start", ZF4_FONT_HOR_ALIGN_CENTER, ZF4_FONT_VER_ALIGN_CENTER);

    refresh_text_positions(&scene->renderer, ts);

    return true;
}

bool title_screen_tick(ZF4Scene* scene, int* sceneChangeIndex, ZF4GamePtrs* gamePtrs) {
    TitleScreen* ts = scene->userData;

    if (zf4_is_key_down(ZF4_KEY_ENTER)) {
        *sceneChangeIndex = WORLD_SCENE;
    }

    refresh_text_positions(&scene->renderer, ts);

    return true;
}
