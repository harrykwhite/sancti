#pragma once

#include <zf3.h>

enum TitleScreenRenderLayer {
    MAIN_TS_RENDER_LAYER,
    NUM_TS_RENDER_LAYERS
};

struct TitleScreen {
    zf3::CharBatchID titleTextCBID;
    zf3::CharBatchID startTextCBID;
};

void init_title_screen(TitleScreen& ts, const zf3::UserGameFuncData& zf3Data);
void title_screen_tick(TitleScreen& ts, const zf3::UserGameFuncData& zf3Data);
