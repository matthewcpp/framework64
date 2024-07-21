#pragma once

#include "framework64/camera.h"
#include "framework64/engine.h"
#include "framework64/scene.h"

typedef struct lines_example {
    fw64Engine* engine;
    float rotation;
    fw64Scene scene;
    fw64RenderPass* renderpass;
} Game;

#ifdef __cplusplus
extern "C" {
#endif

void game_init(Game* example, fw64Engine* engine);
void game_update(Game* example);
void game_draw(Game* example);

#ifdef __cplusplus
}
#endif
