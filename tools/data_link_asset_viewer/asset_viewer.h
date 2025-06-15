#pragma once

#include "framework64/data_link.h"
#include "framework64/engine.h"

#include "fw64_asset_viewer.h"
#include "fw64_file_downloader.h"

typedef struct {
    fw64Engine* engine;
    fw64DataLink* data_link;
    fw64Font* font;
    fw64RenderPass* renderpass;
    fw64AssetViewer asset_viewer;
    fw64FileDownloader file_downloader;
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
