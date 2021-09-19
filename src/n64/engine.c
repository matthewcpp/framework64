#include "framework64/engine.h"
#include "assets.h"

#include "framework64/n64/audio.h"
#include "framework64/n64/assets.h"
#include "framework64/n64/input.h"
#include "framework64/n64/renderer.h"

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
fw64Assets assets;
fw64Input input;
fw64Renderer renderer;
fw64Time time;
u64 _previous_update_time = 0;

static void fw64_n64_engine_update_time(fw64Engine* engine) {
    u64 current_ms = OS_CYCLES_TO_USEC(osGetTime()) / 1000;

    if (current_ms < _previous_update_time)
        return;

    float ms_delta = (float)(current_ms - _previous_update_time);
    engine->time->time_delta = ms_delta / 1000.0f;

    _previous_update_time = current_ms;
}

int fw64_n64_engine_init(fw64Engine* system) {
    system->audio = &audio;
    system->assets = &assets;
    system->input = &input;
    system->renderer = &renderer;
    system->time = &time;

    InitHeap(memory_heap, FW64_N64_HEAP_SIZE);

    nuGfxInit(); // starts nusys graphics
    nuAuInit(); //starts the SGI tools audio manager

    fw64_n64_renderer_init(system->renderer, FW64_N64_SCREEN_WIDTH, FW64_N64_SCREEN_HEIGHT);
    fw64_n64_input_init(system->input);
    memset(system->time, 0, sizeof(fw64Time));
    fw64_n64_assets_init(system->assets);
    fw64_n64_audio_init(system->audio);

    fw64_n64_filesystem_init(FW64_ASSET_COUNT);

    return 1;
}

void fw64_n64_engine_update(fw64Engine* engine) {
    fw64_n64_engine_update_time(engine);
    fw64_n64_input_update(engine->input);
}
