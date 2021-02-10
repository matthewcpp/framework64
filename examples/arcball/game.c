#include "game.h"

#include "assets.h"

#include "framework64/entity.h"
#include "framework64/rect.h"

#include <malloc.h>
#include <string.h>
#include <stdio.h>

#define ENTITY_N64_LOGO 0
#define ENTITY_SUZANNE 1
#define ENTITY_PENGUIN 2

#define SWITCH_MODEL_TEXT "Switch Model"
#define ORBIT_CAMERA_TEXT "Orbit"
#define ZOOM_CAMERA_TEXT "ZOOM"

void game_init(Game* game, fw64System* system) {
    game->system = system;

    camera_init(&game->camera);
    arcball_init(&game->arcball, &game->camera, system->input);

    n64logo_init(&game->n64logo);
    suzanne_init(&game->suzanne);
    penguin_init(&game->penguin);

    game->current_entity = 0;
    game->entities[ENTITY_N64_LOGO] = &game->n64logo.entity;
    game->entities[ENTITY_SUZANNE] = &game->suzanne.entity;
    game->entities[ENTITY_PENGUIN] = &game->penguin.entity;
    
    Entity* currentEntity = game->entities[game->current_entity];
    arcball_set_initial(&game->arcball, &currentEntity->bounding);

    game->camera.near = 4.0f;
    game->camera.far = 1000.0f;
    camera_update_projection_matrix(&game->camera);

    Color fill_color = {255, 0, 0};
    renderer_set_fill_color(system->renderer, &fill_color);

    font_load(ASSET_font_Consolas12, &game->consolas);
    sprite_load(ASSET_sprite_buttons, &game->button_sprite);

    IVec2 text_measurement = font_measure_text(&game->consolas, SWITCH_MODEL_TEXT);
    game->switch_model_text_width = text_measurement.x;
}

void game_update(Game* game, float time_delta) {
    int previous_entity = game->current_entity;

    if (input_button_pressed(game->system->input, 0, R_CBUTTONS)) {
        game->current_entity += 1;
        if (game->current_entity >= ENTITY_COUNT)
            game->current_entity = 0;
    }

        if (input_button_pressed(game->system->input, 0, L_CBUTTONS)) {
        game->current_entity -= 1;
        if (game->current_entity < 0)
            game->current_entity = ENTITY_COUNT - 1;
    }

    if (previous_entity != game->current_entity) {
        arcball_set_initial(&game->arcball, &game->entities[game->current_entity]->bounding);
    }

    if (game->current_entity == ENTITY_PENGUIN && input_button_pressed(game->system->input, 0, Z_TRIG))
        game->penguin.draw_with_lighting = !game->penguin.draw_with_lighting;

    arcball_update(&game->arcball, time_delta);
}

void game_draw(Game* game) {
    Renderer* renderer = game->system->renderer;
    renderer_begin(renderer, &game->camera, RENDERER_MODE_TRIANGLES, RENDERER_FLAG_CLEAR);

    switch (game->current_entity) {
        case ENTITY_N64_LOGO:
            n64logo_draw(&game->n64logo, renderer);
        break;

        case ENTITY_SUZANNE:
            suzanne_draw(&game->suzanne, renderer);
        break;

        case ENTITY_PENGUIN:
            penguin_draw(&game->penguin, renderer);
        break;
    }

    renderer_begin_2d(renderer);
    renderer_set_sprite_mode(renderer);

    IVec2 screen_size;
    renderer_get_screen_size(renderer, &screen_size);

    int button_width = image_sprite_get_slice_width(&game->button_sprite);
    int draw_pos_x = 10;
    int draw_pos_y = 200;

    renderer_draw_sprite_slice(renderer, &game->button_sprite, 15, draw_pos_x, draw_pos_y);
    draw_pos_x += button_width + 3;
    renderer_draw_text(renderer, &game->consolas, draw_pos_x, draw_pos_y, ORBIT_CAMERA_TEXT);

    draw_pos_x = screen_size.x - game->switch_model_text_width - 3;
    renderer_draw_text(renderer, &game->consolas, draw_pos_x, draw_pos_y, SWITCH_MODEL_TEXT);

    draw_pos_x -= button_width + 3;
    renderer_draw_sprite_slice(renderer, &game->button_sprite, 5, draw_pos_x, draw_pos_y);

    draw_pos_x -= button_width + 3;
    renderer_draw_sprite_slice(renderer, &game->button_sprite, 4, draw_pos_x, draw_pos_y);

    draw_pos_y = 220;
    draw_pos_x = 10;
    renderer_draw_sprite_slice(renderer, &game->button_sprite, 0, draw_pos_x, draw_pos_y);

    draw_pos_x += button_width + 3;
    renderer_draw_sprite_slice(renderer, &game->button_sprite, 1, draw_pos_x, draw_pos_y);

    draw_pos_x += button_width + 3;
    renderer_draw_text(renderer, &game->consolas, draw_pos_x, draw_pos_y, ZOOM_CAMERA_TEXT);

    renderer_end(renderer, RENDERER_FLAG_SWAP);
}
