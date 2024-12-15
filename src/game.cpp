#include "game.h"

static Game i_game;

static void load_state(const GameState state) {
    zf3::reset_mem_arena(&i_game.memArena);

    i_game.state = state;

    switch (state) {
        case TITLE_SCREEN_GAME_STATE:
            i_game.titleScreen = zf3::push_to_mem_arena<TitleScreen>(&i_game.memArena);
            init_title_screen(*i_game.titleScreen);
            break;

        case WORLD_GAME_STATE:
            i_game.world = zf3::push_to_mem_arena<World>(&i_game.memArena);
            init_world(*i_game.world);
            break;

        default:
            assert(false);
            break;
    }
}

void init_game() {
    if (!zf3::init_mem_arena(&i_game.memArena, zf3::megabytes_to_bytes(4))) {
        // TODO: Handle!
    }

    load_state(TITLE_SCREEN_GAME_STATE);
}

void game_tick() {
    GameState nextState = INVALID_GAME_STATE;

    switch (i_game.state) {
        case TITLE_SCREEN_GAME_STATE:
            title_screen_tick(*i_game.titleScreen, nextState);
            break;

        case WORLD_GAME_STATE:
            world_tick(*i_game.world, nextState);
            break;

        default:
            assert(false);
            break;
    }

    if (nextState != INVALID_GAME_STATE) {
        load_state(nextState);
    }
}

void clean_game() {
    zf3::clean_mem_arena(&i_game.memArena);
}
