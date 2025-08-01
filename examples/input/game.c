#include "game.h"
#include "assets/assets.h"

#include <stdio.h>

#include "framework64/controller_mapping/n64.h"
#include "framework64/util/renderpass_util.h"

static void controller_info_init(ControllerInfo* info, fw64Engine* engine, Assets* assets, int index, float viewport_x, float viewport_y);
static void controller_info_update(ControllerInfo* info);
static void controller_info_draw(ControllerInfo* info);

IVec2 not_connected_text_dimensions;

void game_init(Game* game, fw64Engine* engine) {
    fw64Allocator* allocator = fw64_default_allocator();
    game->engine = engine;

    controller_info_init(&game->controller_infos[0], engine, &game->assets, 0, 0.0f, 0.0f);
    controller_info_init(&game->controller_infos[1], engine, &game->assets, 1, 0.5f, 0.0f);
    controller_info_init(&game->controller_infos[2], engine, &game->assets, 2, 0.0f, 0.5f);
    controller_info_init(&game->controller_infos[3], engine, &game->assets, 3, 0.5f, 0.5f);

    game->assets.buttons = fw64_texture_create_from_image(fw64_assets_load_image(engine->assets, FW64_ASSET_image_buttons, allocator), allocator);
    game->assets.font = fw64_assets_load_font(engine->assets, FW64_ASSET_font_Consolas12, allocator);
    not_connected_text_dimensions = fw64_font_measure_text(game->assets.font, "Not Connected");
}

void game_update(Game* game){
   for (int i = 0; i < FW64_MAX_CONTROLLER_COUNT; i++) {
       controller_info_update(&game->controller_infos[i]);
   }
}

void game_fixed_update(Game* game) {
    (void)game;
}

void game_draw(Game* game) {
    for (int i = 0; i < FW64_MAX_CONTROLLER_COUNT; i++) {
        controller_info_draw(&game->controller_infos[i]);
    }
}

void controller_info_init(ControllerInfo* info, fw64Engine* engine, Assets* assets, int index, float viewport_x, float viewport_y) {
    fw64Display* display = fw64_displays_get_primary(engine->displays);
    fw64Allocator* allocator = fw64_default_allocator(); 

    info->engine = engine;
    info->assets = assets;
    info->player_index = index;
    info->renderpass = fw64_renderpass_create(display, allocator);
    info->spritebatch = fw64_spritebatch_create(1, allocator);

    Vec2 viewport_pos = {viewport_x, viewport_y};
    Vec2 viewport_size = {0.5f, 0.5f};
    fw64Viewport viewport;
    fw64_viewport_set_relative_to_display(&viewport, display, &viewport_pos, &viewport_size);

    fw64_renderpass_set_viewport(info->renderpass, &viewport);
    fw64_renderpass_util_ortho2d(info->renderpass);

}

void controller_info_draw(ControllerInfo* info) {
    fw64_renderpass_begin(info->renderpass);
    fw64_renderpass_draw_sprite_batch(info->renderpass, info->spritebatch);
    fw64_renderpass_end(info->renderpass);

    fw64_renderer_submit_renderpass(info->engine->renderer, info->renderpass);
}

void controller_info_update(ControllerInfo* info) {
    fw64Input* input = info->engine->input;
    const fw64Viewport* viewport = fw64_renderpass_get_viewport(info->renderpass);

    char text[32];
    sprintf(text, "Controller %d", info->player_index);

    IVec2 draw = {5, 5};

    fw64_spritebatch_begin(info->spritebatch);

    fw64_spritebatch_draw_string(info->spritebatch, info->assets->font, text, draw.x, draw.y);
    draw.y += 15;

    if (!fw64_input_controller_is_connected(input, info->player_index)) {
        int text_x = (viewport->size.x / 2) - (not_connected_text_dimensions.x / 2);
        int text_y = (viewport->size.y / 2) - (not_connected_text_dimensions.y / 2);
        fw64_spritebatch_draw_string(info->spritebatch, info->assets->font, "Not Connected", text_x, text_y);
        fw64_spritebatch_end(info->spritebatch);
        return;
    }

    if (fw64_input_controller_has_rumble(input, info->player_index)){
        fw64_spritebatch_draw_string(info->spritebatch, info->assets->font, "Rumble Enabled", draw.x, draw.y);
        draw.y += 15;
    }

    Vec2 stick;
    fw64_input_controller_stick(input, info->player_index, &stick);
    sprintf(text, "Stick: %.2f, %.2f", stick.x, stick.y);
    fw64_spritebatch_draw_string(info->spritebatch, info->assets->font, text, draw.x, draw.y);
    draw.y += 15;

    int pressed_button_count = 0;
    int pressed_buttons[16];

    if (fw64_input_controller_button_down(input, info->player_index, FW64_N64_CONTROLLER_BUTTON_A)) pressed_buttons[pressed_button_count++] = 0;
    if (fw64_input_controller_button_down(input, info->player_index, FW64_N64_CONTROLLER_BUTTON_B)) pressed_buttons[pressed_button_count++] = 1;
    if (fw64_input_controller_button_down(input, info->player_index, FW64_N64_CONTROLLER_BUTTON_C_DOWN)) pressed_buttons[pressed_button_count++] = 2;
    if (fw64_input_controller_button_down(input, info->player_index, FW64_N64_CONTROLLER_BUTTON_C_UP)) pressed_buttons[pressed_button_count++] = 3;
    if (fw64_input_controller_button_down(input, info->player_index, FW64_N64_CONTROLLER_BUTTON_C_LEFT)) pressed_buttons[pressed_button_count++] = 4;
    if (fw64_input_controller_button_down(input, info->player_index, FW64_N64_CONTROLLER_BUTTON_C_RIGHT)) pressed_buttons[pressed_button_count++] = 5;
    if (fw64_input_controller_button_down(input, info->player_index, FW64_N64_CONTROLLER_BUTTON_L)) pressed_buttons[pressed_button_count++] = 6;
    if (fw64_input_controller_button_down(input, info->player_index, FW64_N64_CONTROLLER_BUTTON_R)) pressed_buttons[pressed_button_count++] = 7;
    if (fw64_input_controller_button_down(input, info->player_index, FW64_N64_CONTROLLER_BUTTON_DPAD_DOWN)) pressed_buttons[pressed_button_count++] = 8;
    if (fw64_input_controller_button_down(input, info->player_index, FW64_N64_CONTROLLER_BUTTON_DPAD_UP)) pressed_buttons[pressed_button_count++] = 9;
    if (fw64_input_controller_button_down(input, info->player_index, FW64_N64_CONTROLLER_BUTTON_DPAD_LEFT)) pressed_buttons[pressed_button_count++] = 10;
    if (fw64_input_controller_button_down(input, info->player_index, FW64_N64_CONTROLLER_BUTTON_DPAD_RIGHT)) pressed_buttons[pressed_button_count++] = 11;
    if (fw64_input_controller_button_down(input, info->player_index, FW64_N64_CONTROLLER_BUTTON_START)) pressed_buttons[pressed_button_count++] = 12;
    if (fw64_input_controller_button_down(input, info->player_index, FW64_N64_CONTROLLER_BUTTON_Z)) pressed_buttons[pressed_button_count++] = 13;


    for (int i = 0; i < pressed_button_count; i++) {
        fw64_spritebatch_draw_sprite_slice(info->spritebatch, info->assets->buttons, pressed_buttons[i], draw.x, draw.y);
        draw.x += 17;

        if (draw.x > viewport->size.x - 12) {
            draw.x = 5;
            draw.y += 17;
        }
    }

    fw64_spritebatch_end(info->spritebatch);
}
