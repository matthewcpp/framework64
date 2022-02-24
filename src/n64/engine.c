#include "framework64/engine.h"

#include "framework64/allocator.h"
#include "framework64/n64/audio.h"
#include "framework64/n64/asset_database.h"
#include "framework64/n64/input.h"
#include "framework64/n64/renderer.h"
#include "framework64/n64/save_file.h"

#include "framework64/n64/filesystem.h"

#include <nusys.h>
#include <ultra64.h>

#pragma GCC diagnostic ignored "-Wcomment"
#include <nualsgi_n.h>
#pragma GCC diagnostic pop

#include <malloc.h>
#include <string.h>

#define FW64_N64_SCREEN_WIDTH 320
#define FW64_N64_SCREEN_HEIGHT 240

#define FW64_N64_HEAP_SIZE 1024*512

char memory_heap[FW64_N64_HEAP_SIZE];

fw64Audio audio;
fw64AssetDatabase assets;
fw64Input input;
fw64Renderer renderer;
fw64Time time;
fw64SaveFile save_file;
u64 _previous_update_time = 0;

static void fw64_n64_engine_update_time(fw64Engine* engine) {
    u64 current_ms = OS_CYCLES_TO_USEC(osGetTime()) / 1000;

    if (current_ms < _previous_update_time)
        return;

    float ms_delta = (float)(current_ms - _previous_update_time);
    engine->time->time_delta = ms_delta / 1000.0f;
    engine->time->total_time += engine->time->time_delta;

    _previous_update_time = current_ms;
}

void fw64_n64_swap_func(void* gfxTask)
{
    NUScTask* gfxTaskPtr = (NUScTask*)gfxTask;
    fw64_n64_renderer_swap_func(&renderer, gfxTaskPtr);
    
    osViSwapBuffer(gfxTaskPtr->framebuffer);
}

int fw64_n64_engine_init(fw64Engine* engine, int asset_count) {
    engine->audio = &audio;
    engine->assets = &assets;
    engine->input = &input;
    engine->renderer = &renderer;
    engine->save_file = &save_file;
    engine->time = &time;

    fw64_default_allocator_init();

    InitHeap(memory_heap, FW64_N64_HEAP_SIZE);

    nuGfxInit(); // starts nusys graphics
    nuAuInit(); //starts the SGI tools audio manager
    nuContInit(); // initialize nusys controller subsystem
    nuContRmbMgrInit();
    nuEepromMgrInit();

    nuGfxSwapCfbFuncSet(fw64_n64_swap_func);

    fw64_n64_renderer_init(engine->renderer, FW64_N64_SCREEN_WIDTH, FW64_N64_SCREEN_HEIGHT);
    fw64_n64_input_init(engine->input, engine->time);
    memset(engine->time, 0, sizeof(fw64Time));
    fw64_data_cache_init(&engine->assets->cache);
    fw64_n64_audio_init(engine->audio);
    fw64_n64_save_file_init(engine->save_file);

    fw64_n64_filesystem_init(asset_count);

    return 1;
}

void fw64_n64_engine_update(fw64Engine* engine) {
    fw64_n64_engine_update_time(engine);
    fw64_n64_input_update(engine->input);
    fw64_n64_audio_update(engine->audio);
}
