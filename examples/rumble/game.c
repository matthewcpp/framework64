#include "game.h"
#include "assets/assets.h"

#include "framework64/math.h"
#include "framework64/random.h"
#include "framework64/util/quad.h"

#include "framework64/n64/controller_button.h"

#include <stdio.h>

static void shake_quad(Game* game);
static void set_shake_speed(Game* game);

void game_init(Game* game, fw64Engine* engine) {
    game->engine = engine;
    fw64_camera_init(&game->camera);

    game->rumble_frequency = 0.5f;
    game->rumble_duration = 2.0f;

    fw64_node_init(&game->node);
    fw64_node_set_mesh(&game->node, fw64_textured_quad_create(game->engine, FW64_ASSET_image_n64_logo, NULL));

    game->font = fw64_font_load(engine->assets, FW64_ASSET_font_Consolas12, NULL);
    game->buttons = fw64_texture_create_from_image(fw64_image_load(engine->assets, FW64_ASSET_image_buttons, NULL), NULL);

    set_shake_speed(game);
    game->shake_time = 0.0f;
}

void game_update(Game* game){
    int controller = 0;
    fw64Input* input = game->engine->input;

    if (fw64_input_controller_button_pressed(input, controller, FW64_N64_CONTROLLER_BUTTON_A)) {
        fw64_input_controller_set_rumble(input, 0, game->rumble_frequency, game->rumble_duration);
    }

    if (fw64_input_controller_button_pressed(input, controller, FW64_N64_CONTROLLER_BUTTON_B)) {
        fw64_input_controller_set_rumble(input, 0, 0.0f, 0.0f);
    }

    int controller_is_rumbling = fw64_input_controller_rumble_active(input, controller);

    if (fw64_input_controller_button_pressed(input, controller, FW64_N64_CONTROLLER_BUTTON_DPAD_RIGHT)) {
        game->rumble_frequency = fw64_minf(game->rumble_frequency + 0.1f, 1.0f);
        set_shake_speed(game);
        
        if (controller_is_rumbling)
            fw64_input_controller_set_rumble(input, 0, game->rumble_frequency, game->rumble_duration);
    }

    if (fw64_input_controller_button_pressed(input, controller, FW64_N64_CONTROLLER_BUTTON_DPAD_LEFT)) {
        game->rumble_frequency = fw64_maxf(game->rumble_frequency - 0.1f, 0.0f);
        set_shake_speed(game);

        if (controller_is_rumbling)
            fw64_input_controller_set_rumble(input, 0, game->rumble_frequency, game->rumble_duration);
    }

    if (controller_is_rumbling) {
        shake_quad(game);
    }
    else {
        vec3_zero(&game->node.transform.position);
        fw64_node_update(&game->node);
    }
}

void game_draw(Game* game) {
    fw64Renderer* renderer = game->engine->renderer;
    char text[32];
    IVec2 viewport_size = fw64_renderer_get_viewport_size(renderer, &game->camera);


    fw64_renderer_begin(renderer, FW64_RENDERER_MODE_TRIANGLES, FW64_RENDERER_FLAG_CLEAR);

    fw64_renderer_set_camera(renderer, &game->camera);
    fw64_renderer_draw_static_mesh(renderer, &game->node.transform, game->node.mesh);

    int draw_x = 20, draw_y = 20;
    fw64_renderer_draw_sprite_slice(renderer, game->buttons, 0, draw_x, draw_y);
    draw_x += fw64_texture_slice_width(game->buttons) + 5;
    fw64_renderer_draw_text(renderer, game->font, draw_x, draw_y, "Start");
    draw_x += 40;
    fw64_renderer_draw_sprite_slice(renderer, game->buttons, 1, draw_x, draw_y);
    draw_x += fw64_texture_slice_width(game->buttons) + 5;
    fw64_renderer_draw_text(renderer, game->font, draw_x, draw_y, "Stop");
    draw_x = 20;
    draw_y = viewport_size.y - 40;

    fw64_renderer_draw_sprite_slice(renderer, game->buttons, 10, draw_x, draw_y);
    draw_x += fw64_texture_slice_width(game->buttons) + 5;
    fw64_renderer_draw_sprite_slice(renderer, game->buttons, 11, draw_x, draw_y);
    draw_x += fw64_texture_slice_width(game->buttons) + 5;
    sprintf(text, "frequency: %.2f", game->rumble_frequency);
    fw64_renderer_draw_text(renderer, game->font, draw_x, draw_y, text);
    

    fw64_renderer_end(renderer, FW64_RENDERER_FLAG_SWAP);
}

void shake_quad(Game* game) {
    game->shake_time -= game->engine->time->time_delta;

    if (game->shake_time <= 0.0f) {
        vec3_set(&game->node.transform.position, fw64_random_float_in_range(-0.5, 0.5), fw64_random_float_in_range(-0.5, 0.5), 0.0f);
        fw64_node_update(&game->node);

        game->shake_time = game->shake_speed;
    }
}

static void set_shake_speed(Game* game) {
    game->shake_speed = (1.1f - game->rumble_frequency) / 2.0f;
}
