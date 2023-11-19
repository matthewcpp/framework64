#include "game.h"
#include "assets/assets.h"

#include "framework64/n64/controller_button.h"
#include "framework64/log.h"
#include "framework64/n64/data_link.h"

#include "framework64/data_link.h"

#include <stdio.h>
#include <string.h>

#define FILE_VIEWER_DATA_SIZE (256 * 1024)

static void on_data_link_message(fw64DataSource* message, void* arg);
static void on_data_link_connected(void* arg);

static void on_file_download_begin(fw64FileDownloader* file_downloader, void* arg);
static void on_file_download_progress(fw64FileDownloader* file_downloader, void* arg);
static void on_file_download_complete(fw64FileDownloader* file_downloader, void* arg);

void game_init(Game* game, fw64Engine* engine) {
    game->engine = engine;

    game->consolas = fw64_assets_load_font(engine->assets, FW64_ASSET_font_Consolas14, fw64_default_allocator());
    fw64_renderer_set_anti_aliasing_enabled(engine->renderer, 0);

    fw64_file_viewer_init(&game->file_viewer, engine, game->consolas, FILE_VIEWER_DATA_SIZE);
    fw64_file_downloader_init(&game->file_downloader, engine);


    fw64FileDownloaderCallbacks callbacks;
    callbacks.begin = on_file_download_begin;
    callbacks.progress = on_file_download_progress;
    callbacks.complete = on_file_download_complete;
    fw64_file_downloader_set_callbacks(&game->file_downloader, &callbacks, game);

    fw64_data_link_set_connected_callback(engine->data_link, on_data_link_connected, game);
    fw64_data_link_set_mesage_callback(engine->data_link, on_data_link_message, game);

    fw64_file_viewer_set_empty_state(&game->file_viewer, "No data link connection", NULL);
}

void game_update(Game* game){
    fw64_file_viewer_update(&game->file_viewer);
}

void game_draw(Game* game) {
    fw64_file_viewer_draw(&game->file_viewer);
}

void on_data_link_connected(void* arg) {
    Game* game = (Game*)arg;
    fw64_file_viewer_set_empty_state(&game->file_viewer, "No asset loaded", NULL);
}

static void on_data_link_message(fw64DataSource* message, void* arg) {
    Game* game = (Game*)arg;

    fw64_file_downloader_receive_message(&game->file_downloader, message);
}

static void on_file_download_begin(fw64FileDownloader* file_downloader, void* arg) {
    Game* game = (Game*)arg;

    fw64_file_viewer_set_empty_state(&game->file_viewer, "Loading", file_downloader->current_asset_filename);
}

static void on_file_download_progress(fw64FileDownloader* file_downloader, void* arg) {
    Game* game = (Game*)arg;

    fw64_renderer_set_clear_color(game->engine->renderer, 0, 0, 100);
}

static void on_file_download_complete(fw64FileDownloader* file_downloader, void* arg) {
    Game* game = (Game*)arg;
    fw64_renderer_set_clear_color(game->engine->renderer, 0, 100, 0);
    fw64_file_viewer_load_from_media(&game->file_viewer, file_downloader->current_asset_filepath);
}