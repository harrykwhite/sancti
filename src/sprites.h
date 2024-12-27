#ifndef SPRITES_H
#define SPRITES_H

#include <zf4.h>

typedef enum {
    BLUE_PIXEL_SPRITE,
    PLAYER_SPRITE,
    SWORD_SPRITE,
    ENEMY_SPRITE,
    COMPANION_SPRITE,
    CURSOR_SPRITE,

    SPRITE_CNT
} SpriteIndex;

void load_sprite(ZF4Sprite* sprite, int index);

#endif
