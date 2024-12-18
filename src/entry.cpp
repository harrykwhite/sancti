#include <zf3.h>
#include "game.h"

int main() {
    const zf3::UserGameInfo gameInfo = {
        .init = init_game,
        .tick = game_tick,
        .clean = clean_game,
        .initWindowWidth = 1280,
        .initWindowHeight = 720,
        .windowTitle = "Sancti",
        .windowResizable = true,
        .hideCursor = true
    };

    zf3::start_game(gameInfo);
}
