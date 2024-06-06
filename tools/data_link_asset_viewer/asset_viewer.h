#pragma once

#include "framework64/engine.h"

#include "file_downloader/file_downloader.h"
#include "asset_viewer/asset_viewer.h"

typedef struct {
    fw64Engine* engine;
    fw64Font* consolas;
    fw64AssetViewer file_viewer;
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
