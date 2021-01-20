#include "game.h"

#include "ultra/entity.h"
#include "ultra/rect.h"

#include <nusys.h>
#include <malloc.h>
#include <string.h>

#define ENTITY_N64_LOGO 0
#define ENTITY_SUZANNE 1
#define ENTITY_PENGUIN 2

Game* game_create(Renderer* renderer, Input* input) {
    Game* game = malloc(sizeof(Game));
    game->input = input;
    game->renderer = renderer;

    camera_init(&game->camera);
    arcball_init(&game->arcball, &game->camera, input);

    n64logo_init(&game->n64logo);
    suzanne_init(&game->suzanne);
    penguin_init(&game->penguin);

    game->current_entity = 0;
    game->entities[ENTITY_N64_LOGO] = &game->n64logo.entity;
    game->entities[ENTITY_SUZANNE] = &game->suzanne.entity;
    game->entities[ENTITY_PENGUIN] = &game->penguin.entity;
    
    Entity* currentEntity = game->entities[game->current_entity];
    arcball_set_initial(&game->arcball, &currentEntity->bounding);

    game->texture = texture_load(1);

    game->camera.near = 4.0f;
    game->camera.far = 1000.0f;
    camera_update_projection_matrix(&game->camera);

    Color fill_color = {255, 0, 0};
    renderer_set_fill_color(game->renderer, &fill_color);

    return game;
}

void game_update(Game* game, float time_delta) {
    
    int previous_entity = game->current_entity;

    if (input_button_pressed(game->input, 0, R_CBUTTONS)) {
        game->current_entity += 1;
        if (game->current_entity >= ENTITY_COUNT)
            game->current_entity = 0;
    }

        if (input_button_pressed(game->input, 0, L_CBUTTONS)) {
        game->current_entity -= 1;
        if (game->current_entity < 0)
            game->current_entity = ENTITY_COUNT - 1;
    }

    if (previous_entity != game->current_entity) {
        arcball_set_initial(&game->arcball, &game->entities[game->current_entity]->bounding);
    }

    if (game->current_entity == ENTITY_PENGUIN && input_button_pressed(game->input, 0, Z_TRIG))
        game->penguin.draw_with_lighting = !game->penguin.draw_with_lighting;

    arcball_update(&game->arcball, time_delta);
    
}

void game_draw(Game* game) {
    renderer_begin(game->renderer, &game->camera);

    switch (game->current_entity) {
        case ENTITY_N64_LOGO:
            n64logo_draw(&game->n64logo, game->renderer);
        break;

        case ENTITY_SUZANNE:
            suzanne_draw(&game->suzanne, game->renderer);
        break;

        case ENTITY_PENGUIN:
            penguin_draw(&game->penguin, game->renderer);
        break;
    }

    renderer_begin_2d(game->renderer);
    renderer_set_sprite_mode(game->renderer);
    renderer_draw_sprite(game->renderer, game->texture, 10, 10);
    
    renderer_end(game->renderer);
}
