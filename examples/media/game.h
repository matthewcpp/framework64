#pragma once

#include "framework64/engine.h"
#include "framework64/media.h"

#include "fw64_asset_viewer.h"
#include "fw64_media_file_picker.h"

#define DIR_LISTING_MAX 10

typedef enum {
    STATE_NO_MEDIA_PRESENT,
    STATE_PICKING_FILE,
    STATE_VIEWING
} GameState;

typedef struct {
    fw64Engine* engine;
    fw64Font* font;
    fw64RenderPass* renderpass;
    fw64SpriteBatch* spritebatch;
    fw64MediaFilePicker file_picker;
} MediaFileExplorer;

typedef struct {
    fw64Engine* engine;
    fw64RenderPass* renderpass;
    fw64SpriteBatch* spritebatch;
} NoMediaPresent;

typedef struct {
    fw64Engine* engine;
    fw64RenderPass* renderpass;
    fw64AssetViewer view;
} MediaAssetViewer;

typedef struct {
    fw64Engine* engine;
    fw64Media* media;
    GameState state;
    MediaFileExplorer file_explorer;
    MediaAssetViewer asset_viewer;
    NoMediaPresent no_media_present;
} Game;

#ifdef __cplusplus
extern "C" {
#endif

void game_init(Game* game, fw64Engine* engine);
void game_fixed_update(Game* game);
void game_update(Game* game);
void game_draw(Game* game);

#ifdef __cplusplus
}
#endif
