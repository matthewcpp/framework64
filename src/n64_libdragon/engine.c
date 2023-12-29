#include "framework64/n64_libdragon/engine.h"

#include <libdragon.h>

#include <string.h>
#include <time.h>

int fw64_libdragon_engine_init(fw64LibdragonEngine* engine) {
    fw64_default_allocator_init();
    if (!fw64_n64_libdragon_asset_database_init(&engine->asset_database)) {
        engine->error_str = "Failed to initialize asset database";
        return 0;
    }

    memset(&engine->time, 0, sizeof(fw64Time));

    engine->interface.assets = &engine->asset_database;
    engine->interface.renderer = &engine->renderer;
    engine->interface.time = &engine->time;
    engine->error_str = NULL;

    engine->_previous_update_time = timer_ticks() / (TICKS_PER_SECOND / 1000);

    return 1;
}

void fw64_libdragon_engine_update(fw64LibdragonEngine* engine) {
    uint64_t current_ms = timer_ticks() / (TICKS_PER_SECOND / 1000);

    if (current_ms < engine->_previous_update_time)
        return;

    float ms_delta = (float)(current_ms - engine->_previous_update_time);
    engine->time.time_delta = ms_delta / 1000.0f;
    engine->time.total_time += engine->time.time_delta;

    engine->_previous_update_time = current_ms;
}