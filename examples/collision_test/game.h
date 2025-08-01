#pragma once

#include "bomb_omb_battlefield.h"
#include "ui.h"

#include "fw64_collision_scene_manager.h"
#include "fw64_headlight.h"
#include "fw64_skybox.h"

#include <framework64/util/bump_allocator.h>

typedef struct {
    fw64Engine* engine;
    BombOmbBattlefield battlefield;
    fw64BumpAllocator bump_allocator;
    UI ui;
} Game;

#ifdef __cplusplus
extern "C" {
#endif

void game_init(Game* game, fw64Engine* engine);
void game_fixed_update(Game* game);
void game_update(Game* game);
void game_draw(Game* game);

#ifdef __cplusplus
}
#endif
