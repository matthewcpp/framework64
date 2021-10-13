#pragma once

#include "flame.h"

#include "framework64/engine.h"
#include "framework64/node.h"
#include "framework64/scene.h"
#include "framework64/camera.h"

typedef struct {
    fw64Engine* engine;
    fw64Camera camera;
    fw64Scene* scene;
    fw64Node player;
    Flame flames[2];
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
