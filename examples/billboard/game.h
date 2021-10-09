#pragma once

#include "flame.h"

#include "framework64/util/fps_camera.h"
#include "framework64/util/quad.h"

#include "framework64/engine.h"

typedef struct {
    fw64Engine* engine;
    fw64FpsCamera fps;
    Entity campfire;
    Entity ground;
    Entity moon;
    Flame flame;
} Game;

#ifdef __cplusplus
extern "C" {
#endif

void game_init(Game* game, fw64Engine* engine);
void game_update(Game* game);
void game_draw(Game* game);

#ifdef __cplusplus
}
#endif
