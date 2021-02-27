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

void game_init(Game* game, System* system) {
    game->system = system;

    arcball_init(&game->arcball, system->input);

    game->current_entity = 0;
    game->mesh_assets[0] = ASSET_mesh_n64_logo;
    game->mesh_assets[1] = ASSET_mesh_suzanne;
    game->mesh_assets[2] = ASSET_mesh_penguin;

    entity_init(&game->entity, assets_get_mesh(system->assets, game->mesh_assets[game->current_entity]));
    arcball_set_initial(&game->arcball, &game->entity.bounding);

    game->arcball.camera.near = 4.0f;
    game->arcball.camera.far = 1000.0f;
    camera_update_projection_matrix(&game->arcball.camera);

    Color fill_color = {255, 0, 0};
    renderer_set_fill_color(system->renderer, &fill_color);

    game->consolas = assets_get_font(system->assets, ASSET_font_Consolas12);
    game->button_sprite = assets_get_image(system->assets, ASSET_sprite_buttons);

    IVec2 text_measurement = font_measure_text(game->consolas, SWITCH_MODEL_TEXT);
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
        entity_set_mesh(&game->entity, assets_get_mesh(game->system->assets, game->mesh_assets[game->current_entity]));
        arcball_set_initial(&game->arcball, &game->entity.bounding);
    }

    arcball_update(&game->arcball, time_delta);
}

void game_draw(Game* game) {
    Renderer* renderer = game->system->renderer;
    renderer_begin(renderer, &game->arcball.camera, RENDERER_MODE_TRIANGLES, RENDERER_FLAG_CLEAR);
    renderer_draw_static_mesh(renderer, &game->entity.transform, game->entity.mesh);

    IVec2 screen_size;
    renderer_get_screen_size(renderer, &screen_size);

    int button_width = image_sprite_get_slice_width(game->button_sprite);
    int draw_pos_x = 10;
    int draw_pos_y = 200;

    renderer_draw_sprite_slice(renderer, game->button_sprite, 15, draw_pos_x, draw_pos_y);
    draw_pos_x += button_width + 3;
    renderer_draw_text(renderer, game->consolas, draw_pos_x, draw_pos_y, ORBIT_CAMERA_TEXT);

    draw_pos_x = screen_size.x - game->switch_model_text_width - 3;
    renderer_draw_text(renderer, game->consolas, draw_pos_x, draw_pos_y, SWITCH_MODEL_TEXT);

    draw_pos_x -= button_width + 3;
    renderer_draw_sprite_slice(renderer, game->button_sprite, 5, draw_pos_x, draw_pos_y);

    draw_pos_x -= button_width + 3;
    renderer_draw_sprite_slice(renderer, game->button_sprite, 4, draw_pos_x, draw_pos_y);

    draw_pos_y = 220;
    draw_pos_x = 10;
    renderer_draw_sprite_slice(renderer, game->button_sprite, 0, draw_pos_x, draw_pos_y);

    draw_pos_x += button_width + 3;
    renderer_draw_sprite_slice(renderer, game->button_sprite, 1, draw_pos_x, draw_pos_y);

    draw_pos_x += button_width + 3;
    renderer_draw_text(renderer, game->consolas, draw_pos_x, draw_pos_y, ZOOM_CAMERA_TEXT);

    renderer_end(renderer, RENDERER_FLAG_SWAP);
}
