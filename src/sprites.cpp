#include "sprites.h"

static Sprite i_sprites[SPRITE_CNT];
static bool i_spritesInitialized = false;

static Sprite create_default(const TexIndex texIndex) {
    const zf3::Pt2D texSize = zf3::get_assets().textures.sizes[texIndex];
    return {texIndex, {0, 0, texSize.x, texSize.y}};
}

void init_sprites() {
    assert(!i_spritesInitialized);

    const zf3::Pt2D* const texSizes = zf3::get_assets().textures.sizes;

    i_sprites[PLAYER_SPRITE] = create_default(PLAYER_TEX);
    i_sprites[SWORD_SPRITE] = create_default(SWORD_TEX);
    i_sprites[WANDERER_ENEMY_SPRITE] = {ENEMIES_TEX, {0, 0, 24, 36}};
    i_sprites[PSYCHO_ENEMY_SPRITE] = {ENEMIES_TEX, {24, 0, 20, 24}};
    i_sprites[PROJECTILE_SPRITE] = create_default(PROJECTILE_TEX);
    i_sprites[HITBOX_SPRITE] = create_default(HITBOX_TEX);
    i_sprites[CURSOR_SPRITE] = {UI_TEX, {0, 0, 4, 4}};

    i_spritesInitialized = true;
}

const Sprite& get_sprite(const SpriteIndex index) {
    assert(i_spritesInitialized);
    return i_sprites[index];
}
