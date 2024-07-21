#include "game.h"
#include "assets/assets.h"

#include "framework64/controller_mapping/n64.h"
#include "framework64/util/renderpass_util.h"
#include "framework64/util/texture_util.h"

#include <stdio.h>

static void set_image(Game* game, ImageFormat image_format);
static void change_image(Game* game, int direction);
static void change_palette(Game* game, int direction);

void game_init(Game* game, fw64Engine* engine) {
    fw64Allocator* allocator = fw64_default_allocator();
    fw64Display* display = fw64_displays_get_primary(engine->displays);

    game->engine = engine;

    fw64_ui_navigation_init(&game->ui_nav, engine->input, 0);
    fw64_bump_allocator_init(&game->tex_allocator, 5120);
    fw64_renderer_set_clear_color(engine->renderer, 100, 100, 100);

    game->spritebatch = fw64_spritebatch_create(1, allocator);
    game->renderpass = fw64_renderpass_create(display, allocator);
    fw64_renderpass_util_ortho2d(game->renderpass);

    game->font = fw64_assets_load_font(engine->assets, FW64_ASSET_font_header, allocator);
    game->image_format = IMAGE_FORMAT_NONE;
    game->texture = NULL;
    set_image(game, IMAGE_FORMAT_NONE + 1);
}

void game_update(Game* game){
    fw64_ui_navigation_update(&game->ui_nav, game->engine->time->time_delta);

    if (fw64_ui_navigation_moved_right(&game->ui_nav)) {
        change_image(game, 1);
    } else if (fw64_ui_navigation_moved_left(&game->ui_nav)) {
        change_image(game, -1);
    } else if (fw64_ui_navigation_moved_up(&game->ui_nav)) {
        change_palette(game, 1);
    } else if (fw64_ui_navigation_moved_down(&game->ui_nav)) {
        change_palette(game, -1);
    }
}

void game_draw(Game* game) {
    fw64_renderer_begin(game->engine->renderer, FW64_PRIMITIVE_MODE_TRIANGLES, FW64_CLEAR_FLAG_ALL);

    fw64_renderpass_begin(game->renderpass);
    fw64_renderpass_draw_sprite_batch(game->renderpass, game->spritebatch);
    fw64_renderpass_end(game->renderpass);

    fw64_renderer_submit_renderpass(game->engine->renderer, game->renderpass);

    fw64_renderer_end(game->engine->renderer, FW64_RENDERER_FLAG_SWAP);
}

static void update_spritebatch(Game* game);

void set_image(Game* game, ImageFormat image_format) {
    fw64Allocator* tex_allocator = &game->tex_allocator.interface;

    if (game->texture) {
        fw64_texture_util_delete_tex_and_image(game->texture, game->engine->assets, tex_allocator);
        game->texture = NULL;
    }

    fw64_bump_allocator_reset(&game->tex_allocator);

    game->image_format = image_format;
    game->format_name = "";

    fw64AssetId asset_id = FW64_INVALID_ASSET_ID;

    switch(game->image_format) {
        case IMAGE_FORMAT_RGBA32:
            asset_id = FW64_ASSET_image_rgba32;
            game->format_name = "RGBA32";
            break;

        case IMAGE_FORMAT_RGBA16:
            asset_id = FW64_ASSET_image_rgba16;
            game->format_name = "RGBA16";
            break;

        case IMAGE_FORMAT_CI8:
            asset_id = FW64_ASSET_image_ci8;
            game->format_name = "CI8";
            break;

        case IMAGE_FORMAT_CI4:
            asset_id = FW64_ASSET_image_ci4;
            game->format_name = "CI4";
            break;

        case IMAGE_FORMAT_IA8:
            asset_id = FW64_ASSET_image_ia8;
            game->format_name = "IA8";
            break;

        case IMAGE_FORMAT_IA4:
            asset_id = FW64_ASSET_image_ia4;
            game->format_name = "IA4";
            break;

        case IMAGE_FORMAT_I8:
            asset_id = FW64_ASSET_image_i8;
            game->format_name = "I8";
            break;

        case IMAGE_FORMAT_I4:
            asset_id = FW64_ASSET_image_i4;
            game->format_name = "I4";
            break;

        case IMAGE_FORMAT_NONE:
        case IMAGE_FORMAT_COUNT:
            game->format_name = "Unknown";
            break;
    }

    if (asset_id != FW64_INVALID_ASSET_ID) {
        game->texture = fw64_texture_util_create_from_loaded_image(game->engine->assets, asset_id, tex_allocator);
    }

    update_spritebatch(game);
}

void update_spritebatch(Game* game) {
    fw64_spritebatch_begin(game->spritebatch);

    char header_text[24];
    sprintf(header_text, "Format: %s", game->format_name);
    IVec2 screen_size = fw64_display_get_size(fw64_displays_get_primary(game->engine->displays));
    IVec2 header_size = fw64_font_measure_text(game->font, header_text);
    int x_pos = (screen_size.x / 2) - (header_size.x / 2);
    int y_pos = 15;
    fw64_spritebatch_draw_string(game->spritebatch, game->font, header_text, x_pos, y_pos);

    if (game->texture) {
        x_pos = (screen_size.x / 2) - (fw64_texture_slice_width(game->texture) / 2);
        y_pos = (screen_size.y / 2) - (fw64_texture_slice_height(game->texture) / 2);
        fw64_spritebatch_draw_sprite(game->spritebatch, game->texture, x_pos, y_pos);
    }
        
    fw64_spritebatch_end(game->spritebatch);
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
    update_spritebatch(game);
}
