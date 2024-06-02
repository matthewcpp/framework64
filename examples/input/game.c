#include "game.h"
#include "assets/assets.h"

#include <stdio.h>

#include "framework64/controller_mapping/n64.h"

static void init_controller_info(Game* game, int index, float viewport_x, float viewport_y);
static void display_controller_info(Game* game, int index);

void game_init(Game* game, fw64Engine* engine) {
    fw64Allocator* allocator = fw64_default_allocator();
    game->engine = engine;

    init_controller_info(game, 0, 0.0f, 0.0f);
    init_controller_info(game, 1, 0.5f, 0.0f);
    init_controller_info(game, 2, 0.0f, 0.5f);
    init_controller_info(game, 3, 0.5f, 0.5f);

    fw64_renderer_set_anti_aliasing_enabled(engine->renderer, 0);

    game->buttons = fw64_texture_create_from_image(fw64_assets_load_image(engine->assets, FW64_ASSET_image_buttons, allocator), allocator);
    game->font = fw64_assets_load_font(engine->assets, FW64_ASSET_font_Consolas12, allocator);
    game->not_connected_text_dimensions = fw64_font_measure_text(game->font, "Not Connected");
}

void game_update(Game* game){
    (void)game;
}

void game_draw(Game* game) {
    fw64_renderer_begin(game->engine->renderer, FW64_PRIMITIVE_MODE_TRIANGLES, FW64_RENDERER_FLAG_CLEAR);

   for (int i = 0; i < FW64_MAX_CONTROLLER_COUNT; i++) {
       display_controller_info(game, i);
   }

    fw64_renderer_end(game->engine->renderer, FW64_RENDERER_FLAG_SWAP);
}

void init_controller_info(Game* game, int index, float viewport_x, float viewport_y) {
    ControllerInfo* info = &game->controller_infos[index];

    info->player_index = index;

    fw64Display* display = fw64_displays_get_primary(game->engine->displays);
    fw64_camera_init(&info->camera, display);
    Vec2 viewport_pos = {viewport_x, viewport_y};
    Vec2 viewport_size = {0.5f, 0.5f};
    fw64_camera_set_viewport_relative(&info->camera, &viewport_pos, &viewport_size);
}

void display_controller_info(Game* game, int index) {
    ControllerInfo* info = &game->controller_infos[index];
    fw64Renderer* renderer = game->engine->renderer;
    fw64Input* input = game->engine->input;

    fw64_renderer_set_camera(renderer, &info->camera);

    IVec2 viewport_size = info->camera.viewport.size;

    char text[32];
    sprintf(text, "Controller %d", info->player_index);

    IVec2 draw = {5, 5};

    fw64_renderer_draw_text(renderer, game->font, draw.x, draw.y, text);
    draw.y += 15;

    if (!fw64_input_controller_is_connected(input, index)) {
        int text_x = (viewport_size.x / 2) - (game->not_connected_text_dimensions.x / 2);
        int text_y = (viewport_size.y / 2) - (game->not_connected_text_dimensions.y / 2);
        fw64_renderer_draw_text(renderer, game->font, text_x, text_y, "Not Connected");
        return;
    }

    if (fw64_input_controller_has_rumble(input, index)){
        fw64_renderer_draw_text(renderer, game->font, draw.x, draw.y, "Rumble Enabled");
        draw.y += 15;
    }

    Vec2 stick;
    fw64_input_controller_stick(game->engine->input, index, &stick);
    sprintf(text, "Stick: %.2f, %.2f", stick.x, stick.y);
    fw64_renderer_draw_text(renderer, game->font, draw.x, draw.y, text);
    draw.y += 15;

    int pressed_button_count = 0;
    int pressed_buttons[16];

    if (fw64_input_controller_button_down(input, index, FW64_N64_CONTROLLER_BUTTON_A)) pressed_buttons[pressed_button_count++] = 0;
    if (fw64_input_controller_button_down(input, index, FW64_N64_CONTROLLER_BUTTON_B)) pressed_buttons[pressed_button_count++] = 1;
    if (fw64_input_controller_button_down(input, index, FW64_N64_CONTROLLER_BUTTON_C_DOWN)) pressed_buttons[pressed_button_count++] = 2;
    if (fw64_input_controller_button_down(input, index, FW64_N64_CONTROLLER_BUTTON_C_UP)) pressed_buttons[pressed_button_count++] = 3;
    if (fw64_input_controller_button_down(input, index, FW64_N64_CONTROLLER_BUTTON_C_LEFT)) pressed_buttons[pressed_button_count++] = 4;
    if (fw64_input_controller_button_down(input, index, FW64_N64_CONTROLLER_BUTTON_C_RIGHT)) pressed_buttons[pressed_button_count++] = 5;
    if (fw64_input_controller_button_down(input, index, FW64_N64_CONTROLLER_BUTTON_L)) pressed_buttons[pressed_button_count++] = 6;
    if (fw64_input_controller_button_down(input, index, FW64_N64_CONTROLLER_BUTTON_R)) pressed_buttons[pressed_button_count++] = 7;
    if (fw64_input_controller_button_down(input, index, FW64_N64_CONTROLLER_BUTTON_DPAD_DOWN)) pressed_buttons[pressed_button_count++] = 8;
    if (fw64_input_controller_button_down(input, index, FW64_N64_CONTROLLER_BUTTON_DPAD_UP)) pressed_buttons[pressed_button_count++] = 9;
    if (fw64_input_controller_button_down(input, index, FW64_N64_CONTROLLER_BUTTON_DPAD_LEFT)) pressed_buttons[pressed_button_count++] = 10;
    if (fw64_input_controller_button_down(input, index, FW64_N64_CONTROLLER_BUTTON_DPAD_RIGHT)) pressed_buttons[pressed_button_count++] = 11;
    if (fw64_input_controller_button_down(input, index, FW64_N64_CONTROLLER_BUTTON_START)) pressed_buttons[pressed_button_count++] = 12;
    if (fw64_input_controller_button_down(input, index, FW64_N64_CONTROLLER_BUTTON_Z)) pressed_buttons[pressed_button_count++] = 13;


    for (int i = 0; i < pressed_button_count; i++) {
        fw64_renderer_draw_sprite_slice(renderer, game->buttons, pressed_buttons[i], draw.x, draw.y);
        draw.x += 17;

        if (draw.x > viewport_size.x - 12) {
            draw.x = 5;
            draw.y += 17;
        }
    }
}