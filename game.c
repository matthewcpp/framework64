#include "game.h"


#include "entity.h"
#include "static_model.h"

#include <nusys.h>
#include <malloc.h>
#include <string.h>

Entity entity;

Game* game_create(Renderer* renderer, Input* input) {
    Game* game = malloc(64);
    game->camera = malloc(sizeof(Camera));
    game->arcball = arcball_create(game->camera, input);
    game->input = input;
    game->renderer = renderer;

    entity_init(&entity);
    entity.model = 1;

    camera_init(game->camera);
    Box b;
    static_model_bounding_box(entity.model, &b);
    arcball_set_initial(game->arcball, &b);

    game->camera->near = 4.0f;
    game->camera->far = 1000.0f;
    camera_update_projection_matrix(game->camera);

    return game;
}

void game_update(Game* game, float time_delta) {
    arcball_update(game->arcball, time_delta);
}

void game_draw(Game* game) {
    renderer_begin(game->renderer, game->camera);
    renderer_draw_static(game->renderer, &entity);
    renderer_end(game->renderer);
}
