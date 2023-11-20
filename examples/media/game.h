#pragma once

#include "framework64/engine.h"
#include "file_explorer/file_explorer.h"
#include "asset_viewer/asset_viewer.h"

#define DIR_LISTING_MAX 10

typedef enum {
    STATE_NO_MEDIA_PRESENT,
    STATE_PICKING_FILE,
    STATE_VIEWING
} GameState;

typedef struct {
    fw64Engine* engine;
    fw64Camera camera;
    fw64Font* font;
    fw64Texture* texture;
    fw64FileExplorer file_explorer;
    fw64AssetViewer file_viewer;
    GameState state;
} Game;

#ifdef __cplusplus
extern "C" {
#endif

void game_init(Game* game, fw64Engine* engine);
void game_update(Game* game);
void game_draw(Game* game);

#ifdef __cplusplus
}
#endif
