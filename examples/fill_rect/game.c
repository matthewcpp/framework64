#include "game.h"
#include "assets/assets.h"

#include "framework64/color.h"
#include "framework64/math.h"
#include "framework64/matrix.h"
#include "framework64/util/texture_util.h"
#include "framework64/controller_mapping/n64.h"

#include <stdio.h>

void game_init(Game* game, fw64Engine* engine) {
    fw64Allocator* allocator = fw64_default_allocator();
    game->engine = engine;
    game->logo_texture = fw64_texture_create_from_image(fw64_assets_load_image(engine->assets, FW64_ASSET_image_n64_logo, allocator), allocator);
    game->fill_texture = fw64_texture_util_create_from_loaded_image(engine->assets, FW64_ASSET_image_fill, allocator);
    game->font = fw64_assets_load_font(engine->assets, FW64_ASSET_font_Consolas12, allocator);
    fw64_renderer_set_clear_color(engine->renderer, 100, 149, 237);

    game->sprite_batch = fw64_spritebatch_create(LAYER_COUNT, allocator);

    fw64Display* display = fw64_displays_get_primary(engine->displays);
    Vec2 display_size = fw64_display_get_size_f(display);
    game->renderpass = fw64_renderpass_create(display, allocator);

    float projection[16];
    matrix_ortho2d(projection, 0, display_size.x, display_size.y, 0);
    fw64_renderpass_set_projection_matrix(game->renderpass, projection, NULL);

    game->progress_bar.x = 40;
    game->progress_bar.width = 240;
    game->progress_bar.y = 100;
    game->progress_bar.height = 40;
    game->progress_bar.progress = 0.5f;
}

#define PROGRESS_SPEED 0.3f

void game_update(Game* game){
    if (fw64_input_controller_button_down(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_DPAD_LEFT)) {
        game->progress_bar.progress -= PROGRESS_SPEED * game->engine->time->time_delta;
    }
    if (fw64_input_controller_button_down(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_DPAD_RIGHT)) {
        game->progress_bar.progress += PROGRESS_SPEED * game->engine->time->time_delta;
    }

    game->progress_bar.progress = fw64_clamp(game->progress_bar.progress, 0.0f, 1.0f);
    
    if (game->progress_bar.progress < 0.0f)
        game->progress_bar.progress = 0.0f;

    if (game->progress_bar.progress <= 0.0f)
        game->progress_bar.progress = 0.0f;
}

static void draw_background(Game* game, IVec2* screen_size) {
    int pos_x = (screen_size->x / 2) - (fw64_texture_width(game->logo_texture) / 2);
    int pos_y = (screen_size->y / 2) - (fw64_texture_height(game->logo_texture) / 2);

    fw64_spritebatch_set_active_layer(game->sprite_batch, LAYER_LOGO_TEX);

    fw64_spritebatch_set_color(game->sprite_batch, 255, 192, 203, 255);
    fw64_spritebatch_draw_sprite(game->sprite_batch, game->logo_texture, pos_x, pos_y);
}

static void draw_progress_bar(Game* game) {
    int x = game->progress_bar.x;
    int y = game->progress_bar.y;
    int width = game->progress_bar.width;
    int height = game->progress_bar.height;
    int border = 4;
    fw64ColorRGBA8 color;

    // draw border
    fw64_spritebatch_set_active_layer(game->sprite_batch, LAYER_PROGRESS_BAR);
    fw64_spritebatch_set_color(game->sprite_batch, 255, 255, 255, 255);
    fw64_spritebatch_draw_sprite_slice_rect(game->sprite_batch, game->fill_texture, 0, x, y, width, border); // top
    fw64_spritebatch_draw_sprite_slice_rect(game->sprite_batch, game->fill_texture, 0, x, y + height - border, width, border); // bottom
    fw64_spritebatch_draw_sprite_slice_rect(game->sprite_batch, game->fill_texture, 0, x, y, border, height); //left
    fw64_spritebatch_draw_sprite_slice_rect(game->sprite_batch, game->fill_texture, 0, x + width - border, y, border, height); // right

    // determine size and draw track
    x += border;
    y += border;
    width -= border * 2;
    height -= border * 2;

    fw64_spritebatch_set_color(game->sprite_batch, 55, 55, 55, 200);
    fw64_spritebatch_draw_sprite_slice_rect(game->sprite_batch, game->fill_texture, 0, x, y, width, height);

    // determine width / color and draw bar
    width = (int)((float)width * game->progress_bar.progress);

    if (game->progress_bar.progress >= 0.66f)
        fw64_color_rgba8_set(&color, 0, 255, 0, 255);
    else if (game->progress_bar.progress >= 0.33f)
        fw64_color_rgba8_set(&color, 255, 255, 0, 255);
    else
        fw64_color_rgba8_set(&color, 255, 0, 0, 255);

    fw64_spritebatch_set_color(game->sprite_batch, color.r, color.g, color.b, color.a);
    fw64_spritebatch_draw_sprite_slice_rect(game->sprite_batch, game->fill_texture, 0, x, y, width, height);
}

static void draw_progress_text(Game* game, IVec2* screen_size) {
    int percent = (int)(game->progress_bar.progress * 100.0f);
    char text[16];
    sprintf(&text[0], "%d%%", percent);

    IVec2 dimensions = fw64_font_measure_text(game->font, text);

    int pos_x = (screen_size->x / 2) - (dimensions.x / 2);
    int pos_y = 200;

    fw64_spritebatch_set_active_layer(game->sprite_batch, LAYER_PROGRESS_TEXT);
    fw64_spritebatch_draw_string(game->sprite_batch, game->font, text, pos_x, pos_y);
}

void game_draw(Game* game) {
    fw64Renderer* renderer = game->engine->renderer;

    IVec2 screen_size = fw64_display_get_size(fw64_displays_get_primary(game->engine->displays));
    fw64_spritebatch_begin(game->sprite_batch);
    draw_background(game, &screen_size);
    draw_progress_bar(game);
    draw_progress_text(game, &screen_size);
    fw64_spritebatch_end(game->sprite_batch);

    fw64_renderer_begin(renderer, FW64_PRIMITIVE_MODE_TRIANGLES, FW64_CLEAR_FLAG_ALL);
    fw64_renderpass_begin(game->renderpass);
    fw64_renderpass_draw_sprite_batch(game->renderpass, game->sprite_batch);
    fw64_renderpass_end(game->renderpass);

    fw64_renderer_submit_renderpass(renderer, game->renderpass);
    
    fw64_renderer_end(renderer, FW64_RENDERER_FLAG_SWAP);
}
