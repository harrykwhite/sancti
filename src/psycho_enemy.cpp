#include "enemies.h"

void psycho_init(EnemyEnt& ent, void* const entExt) {
    ent.hp = 3;
}

void psycho_tick(EnemyEnt& ent, void* const entExt) {
    ent.vel *= 0.2f;
    ent.pos += ent.vel;
}
