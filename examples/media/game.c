#include "game.h"
#include "assets/assets.h"

#include "framework64/controller_mapping/n64.h"

#include <stdio.h>
#include <string.h>

#define FILE_VIEWER_SIZE 256 * 1024

static void on_file_picked(const char* path, void* arg);

void game_init(Game* game, fw64Engine* engine) {
    game->engine = engine;
    fw64_camera_init(&game->camera, fw64_displays_get_primary(engine->displays));
    game->texture = 0;

    game->font = fw64_assets_load_font(game->engine->assets, FW64_ASSET_font_Consolas14, fw64_default_allocator());

    if (fw64_media_is_present(game->engine->media)) {
        game->state = STATE_PICKING_FILE;
        fw64_file_explorer_init(&game->file_explorer, engine, game->font, 10);
        fw64_file_explorer_set_file_picked_callback(&game->file_explorer, on_file_picked, game);
        fw64_file_viewer_init(&game->file_viewer, engine, game->font, FILE_VIEWER_SIZE);
    }
    else {
        game->state = STATE_NO_MEDIA_PRESENT;
    }
}

void on_file_picked(const char* path, void* arg) {
    Game* game = (Game*)arg;

    fw64_file_viewer_load_from_media(&game->file_viewer, path);
    game->state = STATE_VIEWING;
}

void game_update(Game* game){
    switch(game->state) {
        case STATE_PICKING_FILE:
            fw64_file_explorer_update(&game->file_explorer);
            break;

        case STATE_VIEWING:
            if (fw64_input_controller_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_START))
                game->state = STATE_PICKING_FILE;

            fw64_file_viewer_update(&game->file_viewer);
            break;

        case STATE_NO_MEDIA_PRESENT:
            break;
    }
}

void game_draw(Game* game) {   
    fw64Renderer* renderer = game->engine->renderer;

    switch(game->state) {
        case STATE_PICKING_FILE:
            fw64_file_explorer_draw(&game->file_explorer);
            break;

        case STATE_VIEWING:
            fw64_file_viewer_draw(&game->file_viewer);
            break;

        case STATE_NO_MEDIA_PRESENT:
            fw64_renderer_begin(renderer, FW64_PRIMITIVE_MODE_TRIANGLES, FW64_RENDERER_FLAG_CLEAR);
            fw64_renderer_draw_text(renderer, game->font, 10, 10, "Media is not present.");
            fw64_renderer_end(renderer, FW64_RENDERER_FLAG_SWAP);
            break;
    }
}
