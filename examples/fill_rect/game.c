#include "game.h"
#include "assets/assets.h"

#include "framework64/color.h"
#include "framework64/math.h"
#include "framework64/n64/controller_button.h"

#include <stdio.h>

void game_init(Game* game, fw64Engine* engine) {
    game->engine = engine;
    fw64_camera_init(&game->camera);
    game->logo_texture = fw64_texture_create_from_image(fw64_image_load(engine->assets, FW64_ASSET_image_n64_logo, NULL), NULL);
    game->font = fw64_font_load(engine->assets, FW64_ASSET_font_Consolas12, NULL);
    fw64_renderer_set_clear_color(engine->renderer, 100, 149, 237);

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

    fw64_renderer_set_fill_color(game->engine->renderer, 255, 192, 203, 255);
    fw64_renderer_draw_sprite(game->engine->renderer, game->logo_texture, pos_x, pos_y);
}

static void draw_progress_bar(Game* game) {
    fw64Renderer* renderer = game->engine->renderer;

    int x = game->progress_bar.x;
    int y = game->progress_bar.y;
    int width = game->progress_bar.width;
    int height = game->progress_bar.height;
    int border = 4;
    fw64ColorRGBA8 color;

    // draw border
    fw64_renderer_set_fill_color(renderer, 255, 255, 255, 255);
    fw64_renderer_draw_filled_rect(renderer, x, y, width, border); // top
    fw64_renderer_draw_filled_rect(renderer, x, y + height - border, width, border); // bottom
    fw64_renderer_draw_filled_rect(renderer, x, y, border, height); //left
    fw64_renderer_draw_filled_rect(renderer, x + width - border, y, border, height); // right

    // determine size and draw track
    x += border;
    y += border;
    width -= border * 2;
    height -= border * 2;

    fw64_renderer_set_fill_color(renderer, 55, 55, 55, 200);
    fw64_renderer_draw_filled_rect(renderer, x, y, width, height);

    // determine width / color and draw bar
    width = (float)(width * game->progress_bar.progress);

    if (game->progress_bar.progress >= 0.66f)
        fw64_color_rgba8_set(&color, 0, 255, 0, 255);
    else if (game->progress_bar.progress >= 0.33f)
        fw64_color_rgba8_set(&color, 255, 255, 0, 255);
    else
        fw64_color_rgba8_set(&color, 255, 0, 0, 255);

    fw64_renderer_set_fill_color(renderer, color.r, color.g, color.b, color.a);
    fw64_renderer_draw_filled_rect(renderer, x, y, width, height);
}

static void draw_progress_text(Game* game, IVec2* screen_size) {
    int percent = (int)(game->progress_bar.progress * 100.0f);
    char text[16];
    sprintf(&text[0], "%d%%", percent);

    IVec2 dimensions = fw64_font_measure_text(game->font, text);

    int pos_x = (screen_size->x / 2) - (dimensions.x / 2);
    int pos_y = 200;

    fw64_renderer_draw_text(game->engine->renderer, game->font, pos_x, pos_y, &text[0]);
}

void game_draw(Game* game) {
    fw64Renderer* renderer = game->engine->renderer;

    IVec2 screen_size;
    fw64_renderer_get_screen_size(renderer, &screen_size);

    fw64_renderer_begin(renderer, FW64_RENDERER_MODE_TRIANGLES, FW64_RENDERER_FLAG_CLEAR);
    fw64_renderer_set_camera(renderer, &game->camera);
    draw_background(game, &screen_size);
    draw_progress_bar(game);
    draw_progress_text(game, &screen_size);
    fw64_renderer_end(renderer, FW64_RENDERER_FLAG_SWAP);
}
