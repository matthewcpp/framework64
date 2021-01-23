#ifndef SPRITES_EXAMPLE_H
#define SPRITES_EXAMPLE_H

#include "ultra/input.h"
#include "ultra/renderer.h"
#include "ultra/camera.h"

#include "sprites.h"

typedef struct {
    Input* input;
    Renderer* renderer;
    Camera camera;

    N64LogoSprite n64logo;
    KenSprite ken_sprite;
} SpritesExample;

void sprites_example_init(SpritesExample* example, Input* input, Renderer* renderer);
void sprites_example_update(SpritesExample* example, float time_delta);
void sprites_example_draw(SpritesExample* example);

#endif