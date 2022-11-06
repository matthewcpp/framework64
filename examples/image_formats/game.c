#include "game.h"
#include "assets/assets.h"

#include "framework64/n64/controller_button.h"

#include <stdio.h>

static void set_image(Game* game, ImageFormat image_format);
static void change_image(Game* game, int direction);

void game_init(Game* game, fw64Engine* engine) {
    game->engine = engine;
    fw64_camera_init(&game->camera);

    fw64_renderer_set_clear_color(engine->renderer, 100, 100, 100);

    game->font = fw64_font_load(engine->assets, FW64_ASSET_font_header, fw64_default_allocator());
    game->image_format = IMAGE_FORMAT_NONE;
    game->texture = fw64_texture_create_from_image(NULL, fw64_default_allocator());
    set_image(game, IMAGE_FORMAT_NONE + 1);

    fw64_renderer_get_screen_size(engine->renderer, &game->screen_size);
}

void game_update(Game* game){
    if (    fw64_input_controller_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_C_RIGHT) || 
            fw64_input_controller_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_DPAD_RIGHT)) {
        change_image(game, 1);
    }
    else if (fw64_input_controller_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_C_LEFT) || 
            fw64_input_controller_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_DPAD_LEFT)) {
        change_image(game, -1);
    }
}

void game_draw(Game* game) {
    fw64Renderer* renderer = game->engine->renderer;

    fw64_renderer_begin(renderer, FW64_PRIMITIVE_MODE_TRIANGLES, FW64_RENDERER_FLAG_CLEAR);
    fw64_renderer_set_camera(renderer, &game->camera);

    int x_pos = (game->screen_size.x / 2) - (game->header_size.x / 2);
    int y_pos = 20;

    fw64_renderer_set_fill_color(renderer, 255, 255, 255, 255);

    fw64_renderer_draw_text(renderer, game->font, x_pos, y_pos, game->header_text);
    
    x_pos = (game->screen_size.x / 2) - (fw64_texture_width(game->texture) / 2);
    y_pos = (game->screen_size.y / 2) - (fw64_texture_height(game->texture) / 2);

    fw64_renderer_set_fill_color(renderer, 0, 255, 0, 255);
    
    fw64_renderer_draw_sprite(renderer, game->texture, x_pos, y_pos);

    fw64_renderer_end(renderer, FW64_RENDERER_FLAG_SWAP);
}


void set_image(Game* game, ImageFormat image_format) {
    fw64Image* image = fw64_texture_get_image(game->texture);

    if (image) {
        fw64_image_delete(game->engine->assets, image, fw64_default_allocator());
    }

    game->image_format = image_format;
    const char* format_name;

    switch(game->image_format) {
        case IMAGE_FORMAT_IA8:
            format_name = "IA8";
            fw64_texture_set_image(game->texture, fw64_image_load(game->engine->assets, FW64_ASSET_image_ia8, fw64_default_allocator()));
            break;

        case IMAGE_FORMAT_IA4:
            format_name = "IA4";
            fw64_texture_set_image(game->texture, fw64_image_load(game->engine->assets, FW64_ASSET_image_ia4, fw64_default_allocator()));
            break;

        case IMAGE_FORMAT_RGBA32:
            fw64_texture_set_image(game->texture, fw64_image_load(game->engine->assets, FW64_ASSET_image_rgba32, fw64_default_allocator()));
            format_name = "RGBA32";
            break;

        case IMAGE_FORMAT_NONE:
        case IMAGE_FORMAT_COUNT:
            break;
    }

    sprintf(&game->header_text[0], "Format: %s", format_name);
    game->header_size = fw64_font_measure_text(game->font, game->header_text);
}

void change_image(Game* game, int direction) {
    ImageFormat new_format = game->image_format + direction;

    if (new_format == IMAGE_FORMAT_NONE)
        new_format = IMAGE_FORMAT_COUNT - 1;
    else if (new_format == IMAGE_FORMAT_COUNT)
        new_format = IMAGE_FORMAT_NONE + 1;

    set_image(game, new_format);
}