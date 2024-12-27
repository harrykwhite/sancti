#ifndef ENT_TYPES_H
#define ENT_TYPES_H

#include <zf4.h>

typedef enum {
    PLAYER_ENT,
    ENEMY_ENT,
    HITBOX_ENT,

    ENT_TYPE_CNT
} EntityTypeIndex;

typedef struct {
    ZF4Vec2D vel;

    int hp;
    int invTime;

    float swordRot;
    float swordRotOffs;
    bool swordRotAxis; // 0 for negative, 1 for positive.
    int swordChargeTime;
} PlayerEntExt;

typedef struct {
    ZF4Vec2D vel;
    int hp;
    int contactDmg;
    int contactStrength;
} EnemyEntExt;

typedef struct {
    ZF4RectF collider;
    int dmg;
    ZF4Vec2D force;
} HitboxEntExt;

void load_ent_type(ZF4EntType* type, int typeIndex);

bool init_player_ent(ZF4Scene* scene, ZF4EntID entID, ZF4GamePtrs* gamePtrs);
bool player_ent_tick(ZF4Scene* scene, ZF4EntID entID, ZF4GamePtrs* gamePtrs);
void hurt_player_ent(ZF4EntID entID, ZF4EntManager* entManager, int dmg, ZF4Vec2D force);

bool init_enemy_ent(ZF4Scene* scene, ZF4EntID entID, ZF4GamePtrs* gamePtrs);
bool enemy_ent_tick(ZF4Scene* scene, ZF4EntID entID, ZF4GamePtrs* gamePtrs);
void hurt_enemy_ent(ZF4EntID entID, ZF4EntManager* entManager, int dmg, ZF4Vec2D force);

bool init_hitbox_ent(ZF4Scene* scene, ZF4EntID entID, ZF4GamePtrs* gamePtrs);
bool hitbox_ent_tick(ZF4Scene* scene, ZF4EntID entID, ZF4GamePtrs* gamePtrs);

inline bool enemy_ent_filter(ZF4EntID entID, ZF4EntManager* entManager) {
    return zf4_get_ent(entID, entManager)->typeIndex == ENEMY_ENT;
}

#endif
