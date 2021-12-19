#include "game.h"
#include "assets.h"

#include <stdio.h>

#include "framework64/n64/controller_button.h"

static void display_controller(Game* game, int index, int pos_x, int pos_y, IVec2* size);

void game_init(Game* game, fw64Engine* engine) {
    game->engine = engine;
    fw64_camera_init(&game->camera);

    fw64_renderer_set_anti_aliasing_enabled(engine->renderer, 0);

    game->buttons = fw64_texture_create_from_image(fw64_image_load(engine->assets, FW64_ASSET_image_buttons, NULL), NULL);
    game->font = fw64_font_load(engine->assets, FW64_ASSET_font_Consolas12, NULL);
}

void game_update(Game* game){
    (void)game;
}

void game_draw(Game* game) {
    fw64_renderer_begin(game->engine->renderer, &game->camera, FW64_RENDERER_MODE_ORTHO2D, FW64_RENDERER_FLAG_CLEAR);

    IVec2 size;
    fw64_renderer_get_screen_size(game->engine->renderer, &size);
    size.x /= 2;
    size.y /= 2;

    display_controller(game, 0, 0, 0, &size);
    display_controller(game, 1, size.x, 0, &size);
    display_controller(game, 2, 0, size.y, &size);
    display_controller(game, 3, size.x, size.y, &size);

    fw64_renderer_end(game->engine->renderer, FW64_RENDERER_FLAG_SWAP);
}

void display_controller(Game* game, int index, int pos_x, int pos_y, IVec2* size) {
    fw64Renderer* renderer = game->engine->renderer;
    fw64Input* input = game->engine->input;

    char text[16];
    sprintf(text, "Controller %d", index);

    IVec2 draw;
    draw.x = 5;
    draw.y = 5;

    fw64_renderer_draw_text(renderer, game->font, pos_x + draw.x, pos_y + draw.y, text);

    if (!fw64_input_controller_is_connected(input, index)) {
        IVec2 dimensions = fw64_font_measure_text(game->font, "Not Connected");
        int text_x = pos_x + (size->x / 2) - (dimensions.x / 2);
        int text_y = pos_y + (size->y / 2) - (dimensions.y / 2);
        fw64_renderer_draw_text(renderer, game->font,text_x, text_y, "Not Connected");
        return;
    }

    draw.y += 15;

    int pressed_button_count = 0;
    int pressed_buttons[16];

    if (fw64_input_button_down(input, index, FW64_N64_CONTROLLER_BUTTON_A)) pressed_buttons[pressed_button_count++] = 0;
    if (fw64_input_button_down(input, index, FW64_N64_CONTROLLER_BUTTON_B)) pressed_buttons[pressed_button_count++] = 1;
    if (fw64_input_button_down(input, index, FW64_N64_CONTROLLER_BUTTON_C_DOWN)) pressed_buttons[pressed_button_count++] = 2;
    if (fw64_input_button_down(input, index, FW64_N64_CONTROLLER_BUTTON_C_UP)) pressed_buttons[pressed_button_count++] = 3;
    if (fw64_input_button_down(input, index, FW64_N64_CONTROLLER_BUTTON_C_LEFT)) pressed_buttons[pressed_button_count++] = 4;
    if (fw64_input_button_down(input, index, FW64_N64_CONTROLLER_BUTTON_C_RIGHT)) pressed_buttons[pressed_button_count++] = 5;
    if (fw64_input_button_down(input, index, FW64_N64_CONTROLLER_BUTTON_L)) pressed_buttons[pressed_button_count++] = 6;
    if (fw64_input_button_down(input, index, FW64_N64_CONTROLLER_BUTTON_R)) pressed_buttons[pressed_button_count++] = 7;
    if (fw64_input_button_down(input, index, FW64_N64_CONTROLLER_BUTTON_DPAD_DOWN)) pressed_buttons[pressed_button_count++] = 8;
    if (fw64_input_button_down(input, index, FW64_N64_CONTROLLER_BUTTON_DPAD_UP)) pressed_buttons[pressed_button_count++] = 9;
    if (fw64_input_button_down(input, index, FW64_N64_CONTROLLER_BUTTON_DPAD_LEFT)) pressed_buttons[pressed_button_count++] = 10;
    if (fw64_input_button_down(input, index, FW64_N64_CONTROLLER_BUTTON_DPAD_RIGHT)) pressed_buttons[pressed_button_count++] = 11;
    if (fw64_input_button_down(input, index, FW64_N64_CONTROLLER_BUTTON_START)) pressed_buttons[pressed_button_count++] = 12;
    if (fw64_input_button_down(input, index, FW64_N64_CONTROLLER_BUTTON_Z)) pressed_buttons[pressed_button_count++] = 13;


    for (int i = 0; i < pressed_button_count; i++) {
        fw64_renderer_draw_sprite_slice(renderer, game->buttons, pressed_buttons[i], pos_x + draw.x, pos_y + draw.y);
        draw.x += 17;

        if (draw.x > size->x - 12) {
            draw.x = 5;
            draw.y += 17;
        }
    }
}