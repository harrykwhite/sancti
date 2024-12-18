#include "title_screen.h"

static zf3::Vec2D calc_title_text_pos(const zf3::Window& window) {
    return (to_vec_2d(window.size) / 2.0f) - zf3::Vec2D {0.0f, 40.0f};
}

static zf3::Vec2D calc_start_text_pos(const zf3::Window& window) {
    return (to_vec_2d(window.size) / 2.0f) + zf3::Vec2D {0.0f, 32.0f};
}

void init_title_screen(TitleScreen& ts, const zf3::UserGameFuncData& zf3Data) {
    zf3::zero_out(ts);

    zf3::reset_renderer(zf3Data.renderer, NUM_TS_RENDER_LAYERS, MAIN_TS_RENDER_LAYER);

    ts.titleTextCBID = zf3::activate_any_char_batch(zf3Data.renderer, 0, 32, 2, calc_title_text_pos(zf3Data.window));
    zf3::write_to_char_batch(zf3Data.renderer, ts.titleTextCBID, "Sancti", zf3::FONT_HOR_ALIGN_CENTER, zf3::FONT_VER_ALIGN_CENTER);

    ts.startTextCBID = zf3::activate_any_char_batch(zf3Data.renderer, 0, 32, 0, (zf3::to_vec_2d(zf3Data.window.size) / 2.0f) + zf3::Vec2D {0.0f, 32.0f});
    zf3::write_to_char_batch(zf3Data.renderer, ts.startTextCBID, "Press Enter to Start", zf3::FONT_HOR_ALIGN_CENTER, zf3::FONT_VER_ALIGN_CENTER);
}

void title_screen_tick(TitleScreen& ts, const zf3::UserGameFuncData& zf3Data) {
    zf3::get_char_batch_display_props(zf3Data.renderer, ts.titleTextCBID).pos = calc_title_text_pos(zf3Data.window);
    zf3::get_char_batch_display_props(zf3Data.renderer, ts.startTextCBID).pos = calc_start_text_pos(zf3Data.window);
}
