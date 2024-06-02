#include "game.h"
#include "assets/assets.h"

#include "framework64/controller_mapping/n64.h"

#include <stdio.h>

static void set_image(Game* game, ImageFormat image_format);
static void change_image(Game* game, int direction);
static void change_palette(Game* game, int direction);

void game_init(Game* game, fw64Engine* engine) {
    game->engine = engine;
    fw64_camera_init(&game->camera, fw64_displays_get_primary(engine->displays));

    fw64_renderer_set_clear_color(engine->renderer, 100, 100, 100);

    game->font = fw64_assets_load_font(engine->assets, FW64_ASSET_font_header, fw64_default_allocator());
    game->image_format = IMAGE_FORMAT_NONE;
    game->texture = fw64_texture_create_from_image(NULL, fw64_default_allocator());
    set_image(game, IMAGE_FORMAT_NONE + 1);

    game->screen_size = fw64_display_get_size(fw64_displays_get_primary(engine->displays));
}

void game_update(Game* game){
    if (fw64_input_controller_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_C_RIGHT) || 
            fw64_input_controller_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_DPAD_RIGHT)) {
        change_image(game, 1);
    }
    else if (fw64_input_controller_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_C_LEFT) || 
            fw64_input_controller_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_DPAD_LEFT)) {
        change_image(game, -1);
    }

    else if (fw64_input_controller_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_C_UP) || 
            fw64_input_controller_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_DPAD_UP)) {
        change_palette(game, 1);
    }
    else if (fw64_input_controller_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_C_DOWN) || 
            fw64_input_controller_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_DPAD_DOWN)) {
        change_palette(game, -1);
    }
}

void game_draw(Game* game) {
    fw64Renderer* renderer = game->engine->renderer;

    fw64_renderer_begin(renderer, FW64_PRIMITIVE_MODE_TRIANGLES, FW64_RENDERER_FLAG_CLEAR);
    fw64_renderer_set_camera(renderer, &game->camera);

    int x_pos = (game->screen_size.x / 2) - (game->header_size.x / 2);
    int y_pos = 20;

    //fw64_renderer_set_fill_color(renderer, 255, 255, 255, 255);

    fw64_renderer_draw_text(renderer, game->font, x_pos, y_pos, game->header_text);
    
    x_pos = (game->screen_size.x / 2) - (fw64_texture_width(game->texture) / 2);
    y_pos = (game->screen_size.y / 2) - (fw64_texture_height(game->texture) / 2);

    //fw64_renderer_set_fill_color(renderer, 0, 255, 0, 255);
    
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
        case IMAGE_FORMAT_RGBA32:
            fw64_texture_set_image(game->texture, fw64_assets_load_image(game->engine->assets, FW64_ASSET_image_rgba32, fw64_default_allocator()));
            format_name = "RGBA32";
            break;

        case IMAGE_FORMAT_CI8:
            fw64_texture_set_image(game->texture, fw64_assets_load_image(game->engine->assets, FW64_ASSET_image_ci8, fw64_default_allocator()));
            format_name = "CI8";
            break;

        case IMAGE_FORMAT_CI4:
            fw64_texture_set_image(game->texture, fw64_assets_load_image(game->engine->assets, FW64_ASSET_image_ci4, fw64_default_allocator()));
            format_name = "CI4";
            break;

        case IMAGE_FORMAT_IA8:
            format_name = "IA8";
            fw64_texture_set_image(game->texture, fw64_assets_load_image(game->engine->assets, FW64_ASSET_image_ia8, fw64_default_allocator()));
            break;

        case IMAGE_FORMAT_IA4:
            format_name = "IA4";
            fw64_texture_set_image(game->texture, fw64_assets_load_image(game->engine->assets, FW64_ASSET_image_ia4, fw64_default_allocator()));
            break;

        case IMAGE_FORMAT_I8:
            format_name = "I8";
            fw64_texture_set_image(game->texture, fw64_assets_load_image(game->engine->assets, FW64_ASSET_image_i8, fw64_default_allocator()));
            break;

        case IMAGE_FORMAT_I4:
            format_name = "I4";
            fw64_texture_set_image(game->texture, fw64_assets_load_image(game->engine->assets, FW64_ASSET_image_i4, fw64_default_allocator()));
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

void change_palette(Game* game, int direction) {
    fw64Image* image = fw64_texture_get_image(game->texture);
    int palette_count = (int)fw64_image_get_palette_count(image);

    if (palette_count <= 0)
        return;

    int palette_index = (int)fw64_texture_get_palette_index(game->texture);

    palette_index += direction;

    if (palette_index < 0) {
        palette_index = palette_count - 1;
    }
    else if (palette_index >= palette_count) {
        palette_index = 0;
    }

    fw64_texture_set_palette_index(game->texture, palette_index);
}
