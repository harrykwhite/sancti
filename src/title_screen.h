#pragma once

#include <zf3.h>
#include "game_states.h"

struct TitleScreen {
    int a;
};

void init_title_screen(TitleScreen& ts, const zf3::UserGameFuncData& zf3Data);
bool title_screen_tick(TitleScreen& ts, const zf3::UserGameFuncData& zf3Data, GameState& nextGameState);
