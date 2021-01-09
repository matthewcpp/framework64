#include "game.h"

#include "cube.h"
#include "entity.h"

#include <nusys.h>
#include <malloc.h>
#include <string.h>

Entity entity;
float rot = 0.0f;
float scale_val = 1;

Game* game_create(Renderer* renderer, Input* input) {
    Game* game = malloc(64);
    game->camera = malloc(sizeof(Camera));
    game->arcball = arcball_create(game->camera, input);
    game->input = input;
    game->renderer = renderer;

    entity_init(&entity);

    camera_init(game->camera);
    Box b;
    memcpy(&b, &cube_bounding[0], sizeof(Box));
    arcball_set_initial(game->arcball, &b);

    return game;
}

void game_update(Game* game, float time_delta) {
    arcball_update(game->arcball, time_delta);
}

void game_draw(Game* game) {
    renderer_begin(game->renderer, game->camera);
    renderer_draw_static(game->renderer, &entity, cube_display_list);
    renderer_end(game->renderer);
}
