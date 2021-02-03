#ifndef LINES_EXAMPLE_H
#define LINES_EXAMPLE_H

#include "ultra/camera.h"
#include "ultra/entity.h"
#include "ultra/font.h"
#include "ultra/input.h"
#include "ultra/renderer.h"

#include "object.h"

typedef enum {
    EXAMPLE_DRAW_MODE_SHADED,
    EXAMPLE_DRAW_MODE_WIREFRAME,
    EXAMPLE_DRAW_MODE_WIREFRAME_ON_SHADED
} DrawMode;

typedef struct lines_example {
    Input* input;
    Renderer* renderer;
    Camera camera;
    DrawMode draw_mode;
    float rotation;
    Entity solid_cube;
    Entity wire_cube;
    Font font;
} LinesExample;

void lines_example_init(LinesExample* example, Input* input, Renderer* renderer);
void lines_example_update(LinesExample* example, float time_delta);
void lines_example_draw(LinesExample* example);


#endif