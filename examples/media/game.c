#include "game.h"
#include "assets/assets.h"

#include "framework64/controller_mapping/n64.h"
#include "framework64/util/renderpass_util.h"
#include "framework64/util/text_util.h"

#include <stdio.h>
#include <string.h>

#define FILE_VIEWER_SIZE 256 * 1024

static void on_file_picked(const char* path, void* arg);

static void media_file_explorer_init(MediaFileExplorer* explorer, fw64Engine* engine, fw64Font* font, fw64MediaFilePickerFilePickedFunc callback, void* arg);
static void media_file_explorer_update(MediaFileExplorer* explorer);
static void media_file_explorer_update_spritebatch(MediaFileExplorer* explorer);
static void media_file_explorer_draw(MediaFileExplorer* explorer);

static void media_asset_viewer_init(MediaAssetViewer* asset_viewer, fw64Engine* engine, fw64Font* font);
static void media_asset_viewer_update(MediaAssetViewer* asset_viewer);
static void media_asset_viewer_draw(MediaAssetViewer* asset_viewer);

static void no_media_present_init(NoMediaPresent* no_media, fw64Engine* engine, fw64Font* font);
static void no_media_present_draw(NoMediaPresent* no_media);

void game_init(Game* game, fw64Engine* engine) {
    game->engine = engine;
    game->media = fw64_media_init(engine);
    fw64Font* font = fw64_assets_load_font(game->engine->assets, FW64_ASSET_font_Consolas14, fw64_default_allocator());

    if (fw64_media_is_present(game->media)) {
        game->state = STATE_PICKING_FILE;
        media_file_explorer_init(&game->file_explorer, engine, font, on_file_picked, game);
        media_asset_viewer_init(&game->asset_viewer, engine, font);
    }
    else {
        no_media_present_init(&game->no_media_present, engine, font);
        game->state = STATE_NO_MEDIA_PRESENT;
    }
}

void on_file_picked(const char* path, void* arg) {
    Game* game = (Game*)arg;

    fw64_asset_viewer_load_from_media(&game->asset_viewer.view, path);
    game->state = STATE_VIEWING;
}

void game_update(Game* game){
    switch(game->state) {
        case STATE_PICKING_FILE:
            media_file_explorer_update(&game->file_explorer);
            break;

        case STATE_VIEWING:
            if (fw64_input_controller_button_released(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_START)) {
                game->state = STATE_PICKING_FILE;
            } else {
                media_asset_viewer_update(&game->asset_viewer);
            }
            break;

        case STATE_NO_MEDIA_PRESENT:
            break;
    }
}

void game_fixed_update(Game* game) {
    (void)game;
}

void game_draw(Game* game) {   
    switch(game->state) {
        case STATE_PICKING_FILE:
            media_file_explorer_draw(&game->file_explorer);
            break;

        case STATE_VIEWING:
            media_asset_viewer_draw(&game->asset_viewer);
            break;

        case STATE_NO_MEDIA_PRESENT:
            no_media_present_draw(&game->no_media_present);
            break;
    }
}

void media_file_explorer_init(MediaFileExplorer* explorer, fw64Engine* engine, fw64Font* font, fw64MediaFilePickerFilePickedFunc callback, void* arg) {
    fw64Allocator* allocator = fw64_default_allocator();

    explorer->engine = engine;
    explorer->font = font;
    fw64_media_file_picker_init(&explorer->file_picker, engine, 10);
    fw64_media_file_picker_set_file_picked_callback(&explorer->file_picker, callback, arg);

    explorer->renderpass = fw64_renderpass_create(fw64_displays_get_primary(engine->displays), allocator);
    fw64_renderpass_util_ortho2d(explorer->renderpass);

    explorer->spritebatch = fw64_spritebatch_create(1, allocator);
    media_file_explorer_update_spritebatch(explorer);
}

void media_file_explorer_update(MediaFileExplorer* explorer) {
    fw64_media_file_picker_update(&explorer->file_picker);

    if (fw64_media_file_picker_did_change(&explorer->file_picker)) {
        media_file_explorer_update_spritebatch(explorer);
    }
}

void media_file_explorer_update_spritebatch(MediaFileExplorer* explorer) {
    fw64_spritebatch_begin(explorer->spritebatch);
    int x_pos = 20;
    int y_pos = 20;

    char cwd_str[fw64_media_file_picker_MAX_PATH_LENGTH + 16];
    sprintf(cwd_str, "cwd: %s", explorer->file_picker.dir_stack.path);

    fw64_spritebatch_draw_string(explorer->spritebatch, explorer->font, cwd_str, x_pos, y_pos);

    fw64FileExplorerItemPage* page = explorer->file_picker.dir_data.pages[explorer->file_picker.dir_data.current_page_index];

    for (int i = 0; i < page->count; i++) {
        fw64FileExplorerItem* item = page->items + i;

        y_pos += 20;

        if (i == explorer->file_picker.dir_data.current_page_selected_item) {
            fw64_spritebatch_set_color(explorer->spritebatch, 255, 255, 0, 255);
            fw64_spritebatch_draw_string(explorer->spritebatch, explorer->font, item->name, x_pos, y_pos);
            fw64_spritebatch_set_color(explorer->spritebatch, 255, 255, 255, 255);
        }
        else {
            fw64_spritebatch_draw_string(explorer->spritebatch, explorer->font, item->name, x_pos, y_pos);
        }
    }

    fw64_spritebatch_end(explorer->spritebatch);
}

void media_file_explorer_draw(MediaFileExplorer* explorer) {
    fw64_renderpass_begin(explorer->renderpass);
    fw64_renderpass_draw_sprite_batch(explorer->renderpass, explorer->spritebatch);
    fw64_renderpass_end(explorer->renderpass);
    
    fw64_renderer_submit_renderpass(explorer->engine->renderer, explorer->renderpass);
}

static void media_asset_viewer_init(MediaAssetViewer* asset_viewer, fw64Engine* engine, fw64Font* font) {
    asset_viewer->engine = engine;
    asset_viewer->renderpass = fw64_renderpass_create(fw64_displays_get_primary(engine->displays), fw64_default_allocator());
    fw64_asset_viewer_init(&asset_viewer->view, engine, asset_viewer->renderpass, font, FILE_VIEWER_SIZE);
}

static void media_asset_viewer_update(MediaAssetViewer* asset_viewer) {
    fw64_asset_viewer_update(&asset_viewer->view);
}

static void media_asset_viewer_draw(MediaAssetViewer* asset_viewer) {
    fw64PrimitiveMode primitive_mode = fw64_asset_viewer_determine_primitive_mode(&asset_viewer->view);
    fw64_renderpass_set_primitive_mode(asset_viewer->renderpass, primitive_mode);
    fw64_asset_viewer_draw(&asset_viewer->view);
    fw64_renderer_submit_renderpass(asset_viewer->engine->renderer, asset_viewer->renderpass);
}

void no_media_present_init(NoMediaPresent* no_media, fw64Engine* engine, fw64Font* font) {
    fw64Allocator* allocator = fw64_default_allocator();
    fw64Display* display = fw64_displays_get_primary(engine->displays);
    IVec2 display_size = fw64_display_get_size(display);
    no_media->engine = engine;
    
    no_media->spritebatch = fw64_spritebatch_create(1, allocator);

    const char* no_media_str = "Media is not present.";
    IVec2 text_pos = fw64_text_util_center_string(font, no_media_str, &display_size);
    fw64_spritebatch_begin(no_media->spritebatch);
    fw64_spritebatch_draw_string(no_media->spritebatch, font, no_media_str, text_pos.x, text_pos.y);
    fw64_spritebatch_end(no_media->spritebatch);

    no_media->renderpass = fw64_renderpass_create(display, allocator);
    fw64_renderpass_util_ortho2d(no_media->renderpass);
    fw64_renderpass_begin(no_media->renderpass);
    fw64_renderpass_draw_sprite_batch(no_media->renderpass, no_media->spritebatch);
    fw64_renderpass_end(no_media->renderpass);
}

void no_media_present_draw(NoMediaPresent* no_media) {
    fw64_renderer_submit_renderpass(no_media->engine->renderer, no_media->renderpass);
}
