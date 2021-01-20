#include "game.h"

#include "ultra/entity.h"
#include "ultra/rect.h"
#include "ultra/static_model.h"

#include <nusys.h>
#include <malloc.h>
#include <string.h>

Entity entity;

static void _reset_arcball(Game* game) {
    Box b;
    static_model_bounding_box(entity.model, &b);
    arcball_set_initial(game->arcball, &b);
}

Game* game_create(Renderer* renderer, Input* input) {
    Game* game = malloc(64);
    game->camera = malloc(sizeof(Camera));
    game->arcball = arcball_create(game->camera, input);
    game->input = input;
    game->renderer = renderer;

    entity_init(&entity);
    entity.model = 1;

    camera_init(game->camera);
    _reset_arcball(game);

    game->texture = texture_load(1);

    game->camera->near = 4.0f;
    game->camera->far = 1000.0f;
    camera_update_projection_matrix(game->camera);

    Color fill_color = {255, 0, 0};
    renderer_set_fill_color(game->renderer, &fill_color);

    return game;
}

void game_update(Game* game, float time_delta) {
    if (input_button_pressed(game->input, 0, START_BUTTON)) {
        switch(entity.model) {
            case 1:
                entity.model = 2;
            break;

            case 2:
                entity.model = 3;
            break;

            case 3:
                entity.model = 4;
            break;

            case 4:
                entity.model = 1;
            break;
        }

        _reset_arcball(game);
    }
    else {
        arcball_update(game->arcball, time_delta);
    }

}

void game_draw(Game* game) {
    renderer_begin(game->renderer, game->camera);

    if (entity.model > 1) {
        renderer_activate_lighting(game->renderer);
    }
    renderer_draw_static(game->renderer, &entity);

    renderer_begin_2d(game->renderer);
    //renderer_set_fill_mode(game->renderer);
    //Rect r = {275, 190, 40, 40};
    //renderer_draw_filled_rect(game->renderer, &r);
    renderer_set_sprite_mode(game->renderer);
    renderer_draw_sprite(game->renderer, game->texture, 10, 10);
    renderer_end(game->renderer);
}
