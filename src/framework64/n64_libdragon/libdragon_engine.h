#pragma once

#include "libdragon_asset_database.h"
#include "libdragon_audio.h"
#include "libdragon_display.h"
#include "libdragon_fmv.h"
#include "libdragon_input.h"
#include "libdragon_renderer.h"

#include "framework64/engine.h"

typedef struct {
    fw64AssetDatabase asset_database;
    fw64Audio audio;
    fw64Displays displays;
    fw64Fmv fmv;
    fw64Input input;
    fw64Renderer renderer;
    fw64Time time;
    fw64Engine interface;
} fw64LibdragonEngine;

void fw64_n64_libdragon_engine_init(fw64LibdragonEngine* engine);
void fw64_n64_libdragon_engine_update(fw64LibdragonEngine* engine);
void fw64_n64_libdragon_engine_draw(fw64LibdragonEngine* engine);