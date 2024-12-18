#include "game.h"

static Game i_game;

bool init_game(const zf3::UserGameFuncData& zf3Data) {
    init_enemy_types();
    init_title_screen(i_game.titleScreen, zf3Data);
    return true;
}

bool game_tick(const zf3::UserGameFuncData& zf3Data) {
    if (i_game.inWorld) {
        if (!world_tick(i_game.world, zf3Data)) {
            return false;
        }
    } else {
        title_screen_tick(i_game.titleScreen, zf3Data);

        if (zf3::is_key_pressed(zf3::KEY_ENTER, zf3Data.inputManager)) {
            i_game.inWorld = true;

            if (!init_world(i_game.world, zf3Data)) {
                return false;
            }
        }
    }

    return true;
}

void clean_game() {
    if (i_game.inWorld) {
        clean_world(i_game.world);
    }
}
