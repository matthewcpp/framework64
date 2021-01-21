#include "arcball_example.h"

#include "ultra/entity.h"
#include "ultra/rect.h"

#include <nusys.h>
#include <malloc.h>
#include <string.h>

#define ENTITY_N64_LOGO 0
#define ENTITY_SUZANNE 1
#define ENTITY_PENGUIN 2

void arcball_example_init(ArcballExample* example, Renderer* renderer, Input* input) {
    example->input = input;
    example->renderer = renderer;

    camera_init(&example->camera);
    arcball_init(&example->arcball, &example->camera, input);

    n64logo_init(&example->n64logo);
    suzanne_init(&example->suzanne);
    penguin_init(&example->penguin);

    example->current_entity = 0;
    example->entities[ENTITY_N64_LOGO] = &example->n64logo.entity;
    example->entities[ENTITY_SUZANNE] = &example->suzanne.entity;
    example->entities[ENTITY_PENGUIN] = &example->penguin.entity;
    
    Entity* currentEntity = example->entities[example->current_entity];
    arcball_set_initial(&example->arcball, &currentEntity->bounding);

    example->camera.near = 4.0f;
    example->camera.far = 1000.0f;
    camera_update_projection_matrix(&example->camera);

    Color fill_color = {255, 0, 0};
    renderer_set_fill_color(example->renderer, &fill_color);
}

void arcball_example_update(ArcballExample* example, float time_delta) {
    
    int previous_entity = example->current_entity;

    if (input_button_pressed(example->input, 0, R_CBUTTONS)) {
        example->current_entity += 1;
        if (example->current_entity >= ENTITY_COUNT)
            example->current_entity = 0;
    }

        if (input_button_pressed(example->input, 0, L_CBUTTONS)) {
        example->current_entity -= 1;
        if (example->current_entity < 0)
            example->current_entity = ENTITY_COUNT - 1;
    }

    if (previous_entity != example->current_entity) {
        arcball_set_initial(&example->arcball, &example->entities[example->current_entity]->bounding);
    }

    if (example->current_entity == ENTITY_PENGUIN && input_button_pressed(example->input, 0, Z_TRIG))
        example->penguin.draw_with_lighting = !example->penguin.draw_with_lighting;

    arcball_update(&example->arcball, time_delta);
}

void arcball_example_draw(ArcballExample* example) {
    renderer_begin(example->renderer, &example->camera);

    switch (example->current_entity) {
        case ENTITY_N64_LOGO:
            n64logo_draw(&example->n64logo, example->renderer);
        break;

        case ENTITY_SUZANNE:
            suzanne_draw(&example->suzanne, example->renderer);
        break;

        case ENTITY_PENGUIN:
            penguin_draw(&example->penguin, example->renderer);
        break;
    }

    renderer_end(example->renderer);
}
