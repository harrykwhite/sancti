#include "game.h"

static Game i_game;

static void load_state(const GameState state, const zf3::UserGameFuncData& zf3Data) {
#if 0
    i_game.state = state;

    switch (state) {
        case TITLE_SCREEN_GAME_STATE:
            i_game.titleScreen = zf3::push_to_mem_arena<TitleScreen>(&i_game.memArena);
            init_title_screen(*i_game.titleScreen, zf3Data);
            break;

        case WORLD_GAME_STATE:
            i_game.world = zf3::push_to_mem_arena<World>(&i_game.memArena);
            init_world(*i_game.world);
            break;

        default:
            assert(false);
            break;
    }
#endif
}

bool init_game(const zf3::UserGameFuncData& zf3Data) {
#if 0
    if (!zf3::init_mem_arena(&i_game.memArena, zf3::megabytes_to_bytes(4))) {
        return false;
    }

    //++zf3Data.renderer.layerCnt;

    load_state(TITLE_SCREEN_GAME_STATE, zf3Data);

#endif

    ++zf3Data.renderer->layerCnt;

    const auto charBatchID = zf3::activate_any_char_batch(*zf3Data.renderer, 0, 32, 0, {zf3Data.window->size.x / 2.0f, zf3Data.window->size.y / 2.0f});
    zf3::write_to_char_batch(*zf3Data.renderer, charBatchID, "Hello, world!", zf3::FONT_HOR_ALIGN_LEFT, zf3::FONT_VER_ALIGN_CENTER, zf3Data.assets->fonts);

    return true;
}

bool game_tick(const zf3::UserGameFuncData& zf3Data) {
#if 0
    bool success = false;

    GameState nextState = INVALID_GAME_STATE;

    switch (i_game.state) {
        case TITLE_SCREEN_GAME_STATE:
            success = title_screen_tick(*i_game.titleScreen, zf3Data, nextState);
            break;

        case WORLD_GAME_STATE:
            success = world_tick(*i_game.world, nextState);
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
#endif
    return true;
}
