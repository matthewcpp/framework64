#ifndef LINES_EXAMPLE_H
#define LINES_EXAMPLE_H

#include "fps_camera.h"

#include "framework64/billboard.h"
#include "framework64/entity.h"
#include "framework64/font.h"
#include "framework64/input.h"
#include "framework64/renderer.h"

#include "object.h"

typedef struct lines_example {
    Input* input;
    Renderer* renderer;
    FpsCamera fps;
    Entity solid_cube;
    BillboardQuad quad;
    ImageSprite sprite;
} BillboardExample;

void lines_example_init(BillboardExample* example, Input* input, Renderer* renderer);
void lines_example_update(BillboardExample* example, float time_delta);
void lines_example_draw(BillboardExample* example);

#endif