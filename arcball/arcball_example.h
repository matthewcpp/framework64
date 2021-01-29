#ifndef GAME_H
#define GAME_H

#include "arcball_camera.h"
#include "ultra/camera.h"
#include "ultra/input.h"
#include "ultra/renderer.h"
#include "ultra/texture.h"
#include "ultra/font.h"

#include "models.h"

#define ENTITY_COUNT 3

typedef struct {
    Renderer* renderer;
    Input* input;

    Camera camera;
    ArcballCamera arcball;

    Penguin penguin;
    Suzanne suzanne;
    N64Logo n64logo;

    Font consolas;
    ImageSprite button_sprite;

    Entity* entities[ENTITY_COUNT];
    int current_entity;

    int switch_model_text_width;
} ArcballExample;

void arcball_example_init(ArcballExample* example, Renderer* renderer, Input* input);
void arcball_example_update(ArcballExample* example, float time_delta);
void arcball_example_draw(ArcballExample* example);

#endif