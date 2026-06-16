#include "asset_viewer.h"
#include "assets/assets.h"

#include "framework64/controller_mapping/n64.h"
#include "framework64/log.h"

#include "framework64/data_link.h"
#include "framework64/media.h"

#include <stdio.h>
#include <string.h>

#define FILE_VIEWER_DATA_SIZE (256 * 1024)

static void on_data_link_message(fw64DataSource* message, void* arg);
static void on_data_link_connected(void* arg);

static void on_file_download_begin(fw64FileDownloader* file_downloader, void* arg);
static void on_file_download_progress(fw64FileDownloader* file_downloader, void* arg);
static void on_file_download_complete(fw64FileDownloader* file_downloader, void* arg);

void game_init(Game* game, fw64Engine* engine) {
    fw64Display* display = fw64_displays_get_primary(engine->displays);
    fw64Allocator* allocator = fw64_default_allocator();

    fw64_media_init(engine);

    game->engine = engine;
    game->data_link = fw64_data_link_init(engine);
    game->font = fw64_assets_load_font(engine->assets, FW64_ASSET_font_Consolas14, allocator);
    game->renderpass = fw64_renderpass_create(display, allocator);

    fw64_asset_viewer_init(&game->asset_viewer, engine, game->renderpass, game->font, FILE_VIEWER_DATA_SIZE);
    fw64_file_downloader_init(&game->file_downloader, engine);

    fw64FileDownloaderCallbacks callbacks;
    callbacks.begin = on_file_download_begin;
    callbacks.progress = on_file_download_progress;
    callbacks.complete = on_file_download_complete;
    fw64_file_downloader_set_callbacks(&game->file_downloader, &callbacks, game);

    fw64_data_link_set_connected_callback(game->data_link, on_data_link_connected, game);
    fw64_data_link_set_mesage_callback(game->data_link, on_data_link_message, game);

    fw64_asset_viewer_set_empty_state(&game->asset_viewer, "No data link connection", NULL);
}

void game_update(Game* game){
    fw64_asset_viewer_update(&game->asset_viewer);
}

void game_fixed_update(Game* game) {
    (void)game;
}

void game_draw(Game* game) {
    fw64_renderpass_set_primitive_mode(game->renderpass, fw64_asset_viewer_determine_primitive_mode(&game->asset_viewer));
    fw64_asset_viewer_draw(&game->asset_viewer);
    fw64_renderer_submit_renderpass(game->engine->renderer, game->renderpass);
}

void on_data_link_connected(void* arg) {
    Game* game = (Game*)arg;
    fw64_asset_viewer_set_empty_state(&game->asset_viewer, "No asset loaded", NULL);
}

static void on_data_link_message(fw64DataSource* message, void* arg) {
    Game* game = (Game*)arg;

    fw64_file_downloader_receive_message(&game->file_downloader, message);
}

static void on_file_download_begin(fw64FileDownloader* file_downloader, void* arg) {
    Game* game = (Game*)arg;

    fw64_asset_viewer_set_empty_state(&game->asset_viewer, "Loading", file_downloader->current_asset_filename);
}

static void on_file_download_progress(fw64FileDownloader* file_downloader, void* arg) {
    (void)file_downloader;
    Game* game = (Game*)arg;

    fw64_renderpass_set_clear_color(game->renderpass, 0, 0, 100);
}

static void on_file_download_complete(fw64FileDownloader* file_downloader, void* arg) {
    Game* game = (Game*)arg;
    fw64_renderpass_set_clear_color(game->renderpass, 0, 100, 0);
    fw64_asset_viewer_load_from_media(&game->asset_viewer, file_downloader->current_asset_filepath);
}
