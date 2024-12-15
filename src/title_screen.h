#pragma once

#include <zf3.h>
#include "game_states.h"

struct TitleScreen {
    int a;
};

void init_title_screen(TitleScreen& ts);
void title_screen_tick(TitleScreen& ts, GameState& nextGameState);
