#include "sprites.h"

#include "asset_indexes.h"

static void load_blue_pixel_sprite_src_rect(ZF4Rect* srcRect, int frameIndex) {
    switch (frameIndex) {
        case 0:
            srcRect->x = 0;
            srcRect->y = 0;
            srcRect->width = 1;
            srcRect->height = 1;
            break;
    }
}

static void load_player_sprite_src_rect(ZF4Rect* srcRect, int frameIndex) {
    switch (frameIndex) {
        case 0:
            srcRect->x = 0;
            srcRect->y = 0;
            srcRect->width = 24;
            srcRect->height = 40;
            break;
    }
}

static void load_sword_sprite_src_rect(ZF4Rect* srcRect, int frameIndex) {
    switch (frameIndex) {
        case 0:
            srcRect->x = 24;
            srcRect->y = 0;
            srcRect->width = 32;
            srcRect->height = 10;
            break;
    }
}

static void load_enemy_sprite_src_rect(ZF4Rect* srcRect, int frameIndex) {
    switch (frameIndex) {
        case 0:
            srcRect->x = 0;
            srcRect->y = 0;
            srcRect->width = 24;
            srcRect->height = 36;
            break;
    }
}

static void load_companion_sprite_src_rect(ZF4Rect* srcRect, int frameIndex) {
    switch (frameIndex) {
        case 0:
            srcRect->x = 0;
            srcRect->y = 0;
            srcRect->width = 22;
            srcRect->height = 36;
            break;
    }
}

static void load_cursor_sprite_src_rect(ZF4Rect* srcRect, int frameIndex) {
    switch (frameIndex) {
        case 0:
            srcRect->x = 0;
            srcRect->y = 0;
            srcRect->width = 4;
            srcRect->height = 4;
            break;
    }
}

void load_sprite(ZF4Sprite* sprite, int index) {
    switch (index) {
        case BLUE_PIXEL_SPRITE:
            *sprite = (ZF4Sprite) {
                .texIndex = PIXELS_TEX,
                .frameCnt = 1,
                .srcRectLoader = load_blue_pixel_sprite_src_rect
            };

            break;

        case PLAYER_SPRITE:
            *sprite = (ZF4Sprite) {
                .texIndex = PLAYER_TEX,
                .frameCnt = 1,
                .srcRectLoader = load_player_sprite_src_rect
            };

            break;

        case SWORD_SPRITE:
            *sprite = (ZF4Sprite) {
                .texIndex = PLAYER_TEX,
                .frameCnt = 1,
                .srcRectLoader = load_sword_sprite_src_rect
            };

            break;

        case ENEMY_SPRITE:
            *sprite = (ZF4Sprite) {
                .texIndex = ENEMIES_TEX,
                .frameCnt = 1,
                .srcRectLoader = load_enemy_sprite_src_rect
            };

            break;

        case COMPANION_SPRITE:
            *sprite = (ZF4Sprite) {
                .texIndex = COMPANIONS_TEX,
                .frameCnt = 1,
                .srcRectLoader = load_companion_sprite_src_rect
            };

            break;

        case CURSOR_SPRITE:
            *sprite = (ZF4Sprite) {
                .texIndex = UI_TEX,
                .frameCnt = 1,
                .srcRectLoader = load_cursor_sprite_src_rect
            };

            break;
    }
}
