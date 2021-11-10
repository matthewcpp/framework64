#pragma once

#include "framework64/camera.h"
#include "framework64/engine.h"
#include "framework64/node.h"

typedef enum {
    EXAMPLE_DRAW_MODE_SHADED,
    EXAMPLE_DRAW_MODE_WIREFRAME,
    EXAMPLE_DRAW_MODE_WIREFRAME_ON_SHADED
} DrawMode;

typedef struct lines_example {
    fw64Engine* engine;
    fw64Camera camera;
    DrawMode draw_mode;
    float rotation;
    fw64Node solid_cube;
    fw64Node wire_cube;
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
