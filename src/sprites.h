#pragma once

#include <zf3.h>
#include "asset_indexes.h"

enum SpriteIndex {
    PLAYER_SPRITE,
    SWORD_SPRITE,
    WANDERER_ENEMY_SPRITE,
    PSYCHO_ENEMY_SPRITE,
    PROJECTILE_SPRITE,
    HITBOX_SPRITE,
    CURSOR_SPRITE,

    SPRITE_CNT
};

struct Sprite {
    TexIndex texIndex;
    zf3::Rect srcRect;
};

void init_sprites();
const Sprite& get_sprite(const SpriteIndex index);
