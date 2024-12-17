#pragma once

#include <zf3.h>
#include "game_states.h"

enum TitleScreenRenderLayer {
    MAIN_TS_RENDER_LAYER,
    NUM_TS_RENDER_LAYERS
};

struct TitleScreen {
    zf3::CharBatchID titleTextCBID;
    zf3::CharBatchID startTextCBID;
};

void init_title_screen(TitleScreen& ts, const zf3::UserGameFuncData& zf3Data);
bool title_screen_tick(TitleScreen& ts, GameState& nextGameState, const zf3::UserGameFuncData& zf3Data);
