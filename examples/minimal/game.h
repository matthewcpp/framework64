#pragma once

#include "framework64/engine.h"
#include "framework64/scene.h"

#include "fw64_arcball_camera.h"

#define KNIGHT_MAX 2

typedef struct {
    fw64Engine* engine;
    fw64RenderPass* renderpass;

    fw64Scene scene;
    fw64ArcballCamera arcball;
    fw64Camera camera;
    size_t asset_index;
    int music;
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
