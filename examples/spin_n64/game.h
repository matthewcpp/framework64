#pragma once

#include "framework64/camera.h"
#include "framework64/engine.h"
#include "framework64/scene.h"

#include "fw64_rotate_node.h"

typedef struct lines_example {
    fw64Engine* engine;
    fw64RenderPass* renderpass;
    fw64RotateNode rotate_node;
    fw64Scene scene;
    
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
