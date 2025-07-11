#pragma once

#include "framework64/engine.h"
#include "framework64/scene.h"

typedef struct {
    fw64Engine* engine;
    fw64RenderPass* renderpass;
    fw64Scene* scene;
    fw64Camera camera;
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
