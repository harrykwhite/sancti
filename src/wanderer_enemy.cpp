#include "enemies.h"

void wanderer_init(EnemyEnt& ent, void* const entExt) {
    ent.hp = 5;
}

void wanderer_tick(EnemyEnt& ent, void* const entExt) {
    ent.vel *= 0.2f;
    ent.pos += ent.vel;
}
