#ifndef SCENE_TYPES_H
#define SCENE_TYPES_H

#include <zf4.h>

typedef enum {
    TITLE_SCREEN_SCENE,
    WORLD_SCENE,

    SCENE_TYPE_CNT
} SceneType;

void load_scene_type_info(ZF4SceneTypeInfo* typeInfo, int typeIndex);

#endif
