#include "title_screen.h"

void init_title_screen(TitleScreen& ts) {
}

bool title_screen_tick(TitleScreen& ts, GameState& nextGameState) {
    if (zf3::is_key_pressed(zf3::KEY_ENTER)) {
        nextGameState = WORLD_GAME_STATE;
    }

    return true;
}
