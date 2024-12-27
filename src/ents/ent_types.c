#include "ent_types.h"

#include <stdalign.h>
#include "../sprites.h"

void load_ent_type(ZF4EntType* type, int typeIndex) {
    switch (typeIndex) {
        case PLAYER_ENT:
            type->extSize = sizeof(PlayerEntExt);
            type->extAlignment = alignof(PlayerEntExt);
            type->init = init_player_ent;
            type->tick = player_ent_tick;
            break;

        case ENEMY_ENT:
            type->extSize = sizeof(EnemyEntExt);
            type->extAlignment = alignof(EnemyEntExt);
            type->init = init_enemy_ent;
            type->tick = enemy_ent_tick;
            break;

        case HITBOX_ENT:
            type->extSize = sizeof(HitboxEntExt);
            type->extAlignment = alignof(HitboxEntExt);
            type->init = init_hitbox_ent;
            type->tick = hitbox_ent_tick;
            break;
    }
}
