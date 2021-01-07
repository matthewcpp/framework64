#include "game.h"

#include <nusys.h>
#include <malloc.h>

#include "cube.h"
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

    camera_init(game->camera);

    return game;
}

void game_update(Game* game, float time_delta) {
    if (input_button_down(game->input, 0, A_BUTTON)) {
        rot += 0.0349066f;
        quat_set_axis_angle(&entity.transform.rotation, 0, 1, 0, rot);
    }

    if (input_button_down(game->input, 0, B_BUTTON)) {
        entity.transform.scale.x += 0.03 * scale_val;
        entity.transform.scale.y += 0.03 * scale_val;
        entity.transform.scale.z += 0.03 * scale_val;

        if (entity.transform.scale.x >= 2.0f || entity.transform.scale.x <= 0.5)
            scale_val *= -1;
    }

    if (input_button_down(game->input, 0, U_JPAD)) {
        game->camera->transform.position.y += 0.05;
    }
    if (input_button_down(game->input, 0, D_JPAD)) {
        game->camera->transform.position.y -= 0.05;
    }

    Vec3 up;
    vec3_zero(&up);
    up.y = 1.0f;
    Vec3 target;
    vec3_zero(&target);

    transform_look_at(&game->camera->transform, &target, &up);
    camera_update_view_matrix(game->camera);
}

void game_draw(Game* game) {
    renderer_begin(game->renderer, game->camera);
    renderer_draw_static(game->renderer, &entity, box_display_list);
    renderer_end(game->renderer);
}
