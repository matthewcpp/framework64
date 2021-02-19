#ifndef GAME_H
#define GAME_H

#include "framework64/camera.h"
#include "framework64/entity.h"
#include "framework64/system.h"

#include "object.h"

typedef enum {
    EXAMPLE_DRAW_MODE_SHADED,
    EXAMPLE_DRAW_MODE_WIREFRAME,
    EXAMPLE_DRAW_MODE_WIREFRAME_ON_SHADED
} DrawMode;

typedef struct lines_example {
    System* system;
    Camera camera;
    DrawMode draw_mode;
    float rotation;
    Entity solid_cube;
    Entity wire_cube;
} Game;

void game_init(Game* example, System* system);
void game_update(Game* example, float time_delta);
void game_draw(Game* example);


#endif