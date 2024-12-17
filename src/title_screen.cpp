#include "title_screen.h"

void init_title_screen(TitleScreen& ts, const zf3::UserGameFuncData& zf3Data) {
    ++zf3Data.renderer->layerCnt;
}

bool title_screen_tick(TitleScreen& ts, const zf3::UserGameFuncData& zf3Data, GameState& nextGameState) {
    if (zf3::is_key_pressed(zf3::KEY_ENTER, *zf3Data.inputManager)) {
        nextGameState = WORLD_GAME_STATE;
    }

    zf3::write_to_sprite_batch(*zf3Data.renderer, 0, 0, zf3Data.assets->textures, {}, {0, 0, 24, 24});

    return true;
}
