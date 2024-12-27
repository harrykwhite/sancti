#include <zf4.h>
#include "sprites.h"
#include "ents/ent_types.h"
#include "scenes/scene_types.h"

int main() {
    ZF4UserGameInfo userGameInfo = {
        .windowInitWidth = 1280,
        .windowInitHeight = 720,
        .windowTitle = "Sancti",
        .windowResizable = true,
        .windowHideCursor = true,

        .spriteCnt = SPRITE_CNT,
        .spriteLoader = load_sprite,

        .entTypeCnt = ENT_TYPE_CNT,
        .entTypeLoader = load_ent_type,

        .sceneTypeCnt = SCENE_TYPE_CNT,
        .sceneTypeInfoLoader = load_scene_type_info
    };

    zf4_start_game(&userGameInfo);
}
