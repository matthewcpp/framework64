#ifndef LINES_EXAMPLE_H
#define LINES_EXAMPLE_H

#include "framework64/billboard.h"
#include "framework64/entity.h"
#include "framework64/font.h"
#include "framework64/input.h"
#include "framework64/renderer.h"

typedef struct {
    Input* input;
    Renderer* renderer;
    Camera camera;
    Font consolas;
} DataExample;

void data_example_init(DataExample* example, Input* input, Renderer* renderer);
void data_example_update(DataExample* example, float time_delta);
void data_example_draw(DataExample* example);

#endif