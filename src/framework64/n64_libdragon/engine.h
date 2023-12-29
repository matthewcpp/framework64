#pragma once

#include "framework64/engine.h"

#include "framework64/n64_libdragon/asset_database.h"
#include "framework64/n64_libdragon/renderer.h"

typedef struct {
    fw64Engine interface;
    fw64AssetDatabase asset_database;
    fw64Renderer renderer;
    fw64Time time;
    const char* error_str;
    uint64_t _previous_update_time;
} fw64LibdragonEngine;

int fw64_libdragon_engine_init(fw64LibdragonEngine* engine);
void fw64_libdragon_engine_update(fw64LibdragonEngine* engine);