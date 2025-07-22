#include "framework64/engine.h"

#include "framework64/allocator.h"
#include "framework64/n64_libultra/audio.h"
#include "framework64/n64_libultra/asset_database.h"
#include "framework64/n64_libultra/display.h"
#include "framework64/n64_libultra/input.h"
#include "framework64/n64_libultra/modules.h"
#include "framework64/n64_libultra/renderer.h"
#include "framework64/n64_libultra/save_file.h"

#include "framework64/n64_libultra/filesystem.h"

#include <nusys.h>
#include <ultra64.h>

#pragma GCC diagnostic ignored "-Wcomment"
#include <nualsgi_n.h>
#pragma GCC diagnostic pop

#include <malloc.h>
#include <stdarg.h>
#include <string.h>

#define FW64_N64_SCREEN_WIDTH 320
#define FW64_N64_SCREEN_HEIGHT 240

#define FW64_N64_HEAP_SIZE 1024*512

char memory_heap[FW64_N64_HEAP_SIZE];

fw64Audio audio;
fw64AssetDatabase assets;
fw64Displays displays;
fw64Input input;
fw64Modules modules;
fw64Renderer renderer;
fw64Time time;
fw64SaveFile save_file;
u64 _previous_time = 0;

static void fw64_n64_libultra_engine_update_time(fw64Engine* engine) {
    u64 current_time = osGetTime();

    u64 delta_time = current_time - _previous_time;
    engine->time->time_delta = ((float)OS_CYCLES_TO_USEC(delta_time)) / 1000000.0f;
    engine->time->total_time += engine->time->time_delta;

    _previous_time = current_time;
}

int fw64_n64_libultra_engine_init(fw64Engine* engine, int asset_count) {
    engine->audio = &audio;
    engine->assets = &assets;
    engine->displays = &displays;
    engine->input = &input;
    engine->modules = &modules;
    engine->renderer = &renderer;
    engine->save_file = &save_file;
    engine->time = &time;

    _previous_time = osGetTime();

    _fw64_default_allocator_init();

    InitHeap(memory_heap, FW64_N64_HEAP_SIZE);

    nuGfxInit(); // starts nusys graphics
    nuAuInit(); //starts the SGI tools audio manager
    nuContInit(); // initialize nusys controller subsystem
    nuContRmbMgrInit();
    nuEepromMgrInit();

    memset(engine->time, 0, sizeof(fw64Time));
    engine->time->fixed_time_delta = 1.0f / 30.0f;

    fw64_n64_asset_database_init(engine->assets);
    fw64_n64_renderer_init(engine->renderer, FW64_N64_SCREEN_WIDTH, FW64_N64_SCREEN_HEIGHT);
    fw64_n64_libultra_displays_init(engine->displays, FW64_N64_SCREEN_WIDTH, FW64_N64_SCREEN_HEIGHT);
    fw64_n64_input_init(engine->input, engine->time);
    fw64_n64_audio_init(engine->audio);
    fw64_n64_save_file_init(engine->save_file);

    fw64_n64_filesystem_init(asset_count);

    return 1;
}

void fw64_n64_libultra_engine_update(fw64Engine* engine) {
    fw64_n64_libultra_engine_update_time(engine);
    fw64_n64_input_update(engine->input);
    fw64_n64_audio_update(engine->audio);
    fw64_n64_libultra_modules_update(engine->modules);
}

void fw64_n64_libultra_engine_finalize_frame(fw64Engine* engine) {
    (void)engine;
    fw64_n64_renderer_end_frame(&renderer);
}
