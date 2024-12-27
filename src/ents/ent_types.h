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

    float swordRot;
    float swordRotOffs;
    bool swordRotAxis; // 0 for negative, 1 for positive.
    int swordChargeTime;
} PlayerEntExt;

typedef struct {
    ZF4Vec2D vel;
    int hp;
} EnemyEntExt;

typedef struct {
    ZF4RectF collider;
    int dmg;
    ZF4Vec2D force;
} HitboxEntExt;

void load_ent_type(ZF4EntType* const type, const int typeIndex);

bool init_player_ent(ZF4Scene* const scene, const ZF4EntID entID, const ZF4GamePtrs* const gamePtrs);
bool player_ent_tick(ZF4Scene* const scene, const ZF4EntID entID, const ZF4GamePtrs* const gamePtrs);

bool init_enemy_ent(ZF4Scene* const scene, const ZF4EntID entID, const ZF4GamePtrs* const gamePtrs);
bool enemy_ent_tick(ZF4Scene* const scene, const ZF4EntID entID, const ZF4GamePtrs* const gamePtrs);
void hurt_enemy_ent(ZF4EntManager* const entManager, const ZF4EntID entID, const int dmg, const ZF4Vec2D force);

bool init_hitbox_ent(ZF4Scene* const scene, const ZF4EntID entID, const ZF4GamePtrs* const gamePtrs);
bool hitbox_ent_tick(ZF4Scene* const scene, const ZF4EntID entID, const ZF4GamePtrs* const gamePtrs);

#endif
