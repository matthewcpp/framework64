#include "game.h"

#include <nusys.h>
#include <malloc.h>

#include "assets.h"
#include "entity.h"

Entity entity;
float rot = 0.0f;
float scale_val = 1;

Game* game_create(Renderer* renderer, Input* input) {
    Game* game = malloc(64);
    game->camera = malloc(sizeof(Camera));
    game->input = input;
    game->renderer = renderer;

    entity_init(&entity);

    IVec2 screen_size;
    renderer_get_screen_size(renderer, & screen_size);


    guOrtho(&game->camera->projection,
    -(float)screen_size.x/2.0F, (float)screen_size.x/2.0F,
    -(float)screen_size.y/2.0F, (float)screen_size.y/2.0F,
    1.0F, 10.0F, 1.0F);

    return game;
}

void game_update(Game* game, float time_delta) {
    if (input_button_down(game->input, 0, A_BUTTON)) {
        rot += 0.0349066f;
        quat_set_axis_angle(&entity.rotation, 0, 0, 1, rot);
    }

    if (input_button_down(game->input, 0, B_BUTTON)) {
        entity.scale.x += 0.03 * scale_val;
        entity.scale.y += 0.03 * scale_val;
        entity.scale.z += 0.03 * scale_val;

        if (entity.scale.x >= 2.0f || entity.scale.x <= 0.5)
            scale_val *= -1;
    }
}

void game_draw(Game* game) {
    renderer_begin(game->renderer, game->camera);
    renderer_draw_static(game->renderer, &entity, quad_display_list);
    renderer_end(game->renderer);
}
