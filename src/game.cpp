#include "game.h"

static Game i_game;

static void load_state(const GameState state, const zf3::UserGameFuncData* const zf3Data) {
    i_game.state = state;
    memset(&i_game.stateData, 0, sizeof(i_game.stateData));

    switch (state) {
        case TITLE_SCREEN_GAME_STATE:
            init_title_screen(&i_game.stateData.titleScreen, zf3Data);
            break;

        case WORLD_GAME_STATE:
            init_world(&i_game.stateData.world, zf3Data);
            break;

        default:
            assert(false);
            break;
    }
}

bool init_game(const zf3::UserGameFuncData* const zf3Data) {
    load_state(TITLE_SCREEN_GAME_STATE, zf3Data);
    return true;
}

bool game_tick(const zf3::UserGameFuncData* const zf3Data) {
    bool success = false;

    GameState nextState = INVALID_GAME_STATE;

    switch (i_game.state) {
        case TITLE_SCREEN_GAME_STATE:
            success = title_screen_tick(&i_game.stateData.titleScreen, zf3Data, &nextState);
            break;

        case WORLD_GAME_STATE:
            success = world_tick(&i_game.stateData.world, zf3Data, &nextState);
            break;

        default:
            assert(false);
            break;
    }

    if (success) {
        if (nextState != INVALID_GAME_STATE) {
            load_state(nextState, zf3Data);
        }
    }

    return success;
}
