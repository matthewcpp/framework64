#include "arcball_example.h"

#include "framework64/entity.h"
#include "framework64/rect.h"

#include <nusys.h>
#include <malloc.h>
#include <string.h>
#include <stdio.h>

#define ENTITY_N64_LOGO 0
#define ENTITY_SUZANNE 1
#define ENTITY_PENGUIN 2

#define SWITCH_MODEL_TEXT "Switch Model"
#define ORBIT_CAMERA_TEXT "Orbit"
#define ZOOM_CAMERA_TEXT "ZOOM"

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

    init_consolas_font(&example->consolas);
    init_buttons_sprite(&example->button_sprite);

    IVec2 text_measurement = font_measure_text(&example->consolas, SWITCH_MODEL_TEXT);
    example->switch_model_text_width = text_measurement.x;
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
    renderer_begin(example->renderer, &example->camera, RENDERER_MODE_TRIANGLES, RENDERER_FLAG_CLEAR);

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

    renderer_begin_2d(example->renderer);
    renderer_set_sprite_mode(example->renderer);

    IVec2 screen_size;
    renderer_get_screen_size(example->renderer, &screen_size);

    int button_width = image_sprite_get_slice_width(&example->button_sprite);
    int draw_pos_x = 10;
    int draw_pos_y = 200;

    renderer_draw_sprite_slice(example->renderer, &example->button_sprite, 15, draw_pos_x, draw_pos_y);
    draw_pos_x += button_width + 3;
    renderer_draw_text(example->renderer, &example->consolas, draw_pos_x, draw_pos_y, ORBIT_CAMERA_TEXT);

    draw_pos_x = screen_size.x - example->switch_model_text_width - 3;
    renderer_draw_text(example->renderer, &example->consolas, draw_pos_x, draw_pos_y, SWITCH_MODEL_TEXT);

    draw_pos_x -= button_width + 3;
    renderer_draw_sprite_slice(example->renderer, &example->button_sprite, 5, draw_pos_x, draw_pos_y);

    draw_pos_x -= button_width + 3;
    renderer_draw_sprite_slice(example->renderer, &example->button_sprite, 4, draw_pos_x, draw_pos_y);

    draw_pos_y = 220;
    draw_pos_x = 10;
    renderer_draw_sprite_slice(example->renderer, &example->button_sprite, 0, draw_pos_x, draw_pos_y);

    draw_pos_x += button_width + 3;
    renderer_draw_sprite_slice(example->renderer, &example->button_sprite, 1, draw_pos_x, draw_pos_y);

    draw_pos_x += button_width + 3;
    renderer_draw_text(example->renderer, &example->consolas, draw_pos_x, draw_pos_y, ZOOM_CAMERA_TEXT);

    renderer_end(example->renderer, RENDERER_FLAG_SWAP);
}
