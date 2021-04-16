#include "framework64/engine.h"

#include "framework64/filesystem.h"

#include <nusys.h>

#pragma GCC diagnostic ignored "-Wcomment"
#include <nualsgi_n.h>
#pragma GCC diagnostic pop

#include <malloc.h>

char memory_heap[FW64_N64_HEAP_SIZE];

fw64Audio audio;
fw64Assets assets;
Input input;
Renderer renderer;
Time time;

int fw64_engine_init(fw64Engine* system) {
    system->audio = &audio;
    system->assets = &assets;
    system->input = &input;
    system->renderer = &renderer;
    system->time = &time;

    InitHeap(memory_heap, FW64_N64_HEAP_SIZE);

    nuGfxInit(); // starts nusys graphics
    nuAuInit(); //starts the SGI tools audio manager

    renderer_init(system->renderer, FW64_N64_SCREEN_WIDTH, FW64_N64_SCREEN_HEIGHT);
    input_init(system->input);
    time_init(system->time);
    fw64_assets_init(system->assets);
    fw64_audio_init(system->audio);

    filesystem_init();

    return 1;
}

void fw64_engine_update(fw64Engine* system) {
    time_update(system->time);
    input_update(system->input);
}