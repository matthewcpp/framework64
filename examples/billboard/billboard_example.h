#ifndef LINES_EXAMPLE_H
#define LINES_EXAMPLE_H

#include "fps_camera.h"

#include "framework64/billboard.h"
#include "framework64/entity.h"
#include "framework64/font.h"
#include "framework64/input.h"
#include "framework64/renderer.h"

#include "object.h"

typedef struct {
    Input* input;
    Renderer* renderer;
    FpsCamera fps;
    Entity solid_cube;
    BillboardQuad nintendo_seal_quad;
    ImageSprite nintendo_seal_sprite;
    BillboardQuad n64_logo_quad;
    ImageSprite n64_logo_sprite;
} BillboardExample;

void lines_example_init(BillboardExample* example, Input* input, Renderer* renderer);
void lines_example_update(BillboardExample* example, float time_delta);
void lines_example_draw(BillboardExample* example);

#endif