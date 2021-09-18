#ifndef GAME_H
#define GAME_H

#include "entity.h"

#include "framework64/camera.h"
#include "framework64/engine.h"

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
    Entity solid_cube;
    Entity wire_cube;
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


#endif