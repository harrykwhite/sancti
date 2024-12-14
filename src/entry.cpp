#include <zf3.h>
#include "game.h"

int main() {
    const zf3::UserGameInfo gameInfo = {
        .init = init_game,
        .tick = run_game_tick,
        .cleanup = clean_game,
        .initWindowWidth = 1280,
        .initWindowHeight = 720,
        .windowTitle = "Example ZF3 Game",
        .windowResizable = true
    };

    zf3::run_game(&gameInfo);
}
