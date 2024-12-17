#include "world.h"

#include "game.h"

static Enemy* spawn_enemy(World* const world, const zf3::Vec2D pos) {
    const int enemyIndex = zf3::get_first_inactive_bit_index(world->enemies.activity);

    if (enemyIndex == -1) {
        zf3::log_error("Failed to spawn enemy as all enemy slots are taken!");
        return nullptr;
    }

    zf3::activate_bit(world->enemies.activity, enemyIndex);

    world->enemies[enemyIndex] = {
        .pos = pos
    };

    return &world->enemies[enemyIndex];
}

static zf3::RectFloat get_player_collider(const Player& player, const zf3::Textures& textures) {
    const zf3::Vec2D size = textures.sizes[PLAYER_TEX];
    const zf3::Vec2D topLeft = player.pos - (size / 2.0f);

    return {
        topLeft.x,
        topLeft.y,
        size.x,
        size.y
    };
}

static void proc_player_movement(Player* const player, const zf3::InputManager* const inputManager) {
    const zf3::Vec2D moveAxis = {
        static_cast<float>(zf3::is_key_down(zf3::KEY_D, inputManager)) - static_cast<float>(zf3::is_key_down(zf3::KEY_A, inputManager)),
        static_cast<float>(zf3::is_key_down(zf3::KEY_S, inputManager)) - static_cast<float>(zf3::is_key_down(zf3::KEY_W, inputManager))
    };

    const float moveSpd = 3.0f;

    const zf3::Vec2D velTarg = moveAxis * moveSpd;
    player->vel = zf3::lerp(player->vel, velTarg, 0.25f);
    player->pos += player->vel;
}

static void hurt_player(Player* const player, const zf3::Vec2D force) {
    if (player->invTime > 0) {
        return;
    }
    
    player->vel += force;
    player->invTime = gk_playerInvTime;
}

static zf3::RectFloat get_enemy_collider(const Enemy& enemy, const zf3::Textures& textures) {
    const zf3::Vec2D size = textures.sizes[ENEMY_TEX];
    const zf3::Vec2D topLeft = enemy.pos - (size / 2.0f);

    return {
        topLeft.x,
        topLeft.y,
        size.x,
        size.y
    };
}

static void proc_player_and_enemy_collisions(Player* const player, EnemyActivityBuf* const enemies, const zf3::Textures& textures) {
    const zf3::RectFloat playerCollider = get_player_collider(*player, textures);

    for (int i = 0; i < gk_enemyLimit; ++i) {
        if (!zf3::is_bit_active(enemies->activity, i)) {
            continue;
        }

        const zf3::RectFloat enemyCollider = get_enemy_collider((*enemies)[i], textures);

        if (zf3::do_rects_intersect(playerCollider, enemyCollider)) {
            hurt_player(player, zf3::calc_normal(player->pos - (*enemies)[i].pos) * 12.0f);
        }
    }
}

static void camera_tick(zf3::Camera* const cam, const Player& player, const zf3::InputManager& inputManager, const zf3::Window& window) {
    static constexpr float lk_lookDistLimit = 24.0f;
    static constexpr float lk_lookDistScalarDist = lk_lookDistLimit * 32.0f;

    // Determine the look offset.
    const zf3::Vec2D mouseCamPos = zf3::camera_to_screen_pos(inputManager.inputState.mousePos, cam, &window);
    const float playerToMouseCamPosDist = zf3::calc_dist(player.pos, mouseCamPos);
    const zf3::Vec2D playerToMouseCamPosDir = zf3::calc_normal(mouseCamPos - player.pos);

    const float lookDist = lk_lookDistLimit * zf3::min(playerToMouseCamPosDist / lk_lookDistScalarDist, 1.0f);
    const zf3::Vec2D lookOffs = playerToMouseCamPosDir * lookDist;

    // Determine and approach the target position.
    const zf3::Vec2D targPos = player.pos + lookOffs;
    cam->pos = zf3::lerp(cam->pos, targPos, 0.25f);
}

void init_world(World* const world, const zf3::UserGameFuncData* const zf3Data) {
    zf3::reset_renderer(zf3Data->renderer, NUM_WORLD_RENDER_LAYERS, UI_WORLD_RENDER_LAYER, {0.59f, 0.79f, 0.93f});

    world->player.pos = zf3Data->window->size / 4.0f;
    zf3Data->renderer->cam.pos = world->player.pos;
}

bool world_tick(World* const world, const zf3::UserGameFuncData* const zf3Data, GameState* const nextGameState) {
    proc_player_movement(&world->player, zf3Data->inputManager);

    if (world->player.invTime > 0) {
        --world->player.invTime; // NOTE: Setting this to 1 actually gives you no invincibility time. A bit misleading?
    }

    if (world->enemySpawnTime > 0) {
        --world->enemySpawnTime;
    } else {
        const zf3::Vec2D spawnPos = {
            zf3::gen_rand_float(0.0f, zf3Data->window->size.x / 2.0f),
            zf3::gen_rand_float(0.0f, zf3Data->window->size.y / 2.0f)
        };

        spawn_enemy(world, spawnPos);

        world->enemySpawnTime = gk_enemySpawnInterval;
    }

    proc_player_and_enemy_collisions(&world->player, &world->enemies, zf3Data->assets->textures);

    camera_tick(&zf3Data->renderer->cam, world->player, *zf3Data->inputManager, *zf3Data->window);

    //
    // Writing Render Data
    //

    // Player
    {
        float alpha = 1.0f;

        if (world->player.invTime > 0) {
            alpha = (world->player.invTime & 1) == (gk_playerInvTime & 1) ? 0.5f : 0.75f;
        }

        zf3::write_to_sprite_batch(zf3Data->renderer, ENTS_WORLD_RENDER_LAYER, PLAYER_TEX, &zf3Data->assets->textures, world->player.pos, {0, 0, 24, 40}, {0.5f, 0.5f}, 0.0f, {1.0f, 1.0f}, alpha);
    }

    // Enemies
    for (int i = 0; i < gk_enemyLimit; ++i) {
        if (!zf3::is_bit_active(world->enemies.activity.bytes, i)) {
            continue;
        }

        zf3::write_to_sprite_batch(zf3Data->renderer, ENTS_WORLD_RENDER_LAYER, ENEMY_TEX, &zf3Data->assets->textures, world->enemies[i].pos, {0, 0, 24, 36});
    }

    // Cursor
    zf3::write_to_sprite_batch(zf3Data->renderer, UI_WORLD_RENDER_LAYER, CURSOR_TEX, &zf3Data->assets->textures, zf3Data->inputManager->inputState.mousePos, {0, 0, 4, 4}, {0.5f, 0.5f}, 0.0f, {zf3Data->renderer->cam.scale, zf3Data->renderer->cam.scale});

    return true;
}
