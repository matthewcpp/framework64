#include "framework64/system.h"

#include "framework64/filesystem.h"

#include <nusys.h>

#pragma GCC diagnostic ignored "-Wcomment"
#include <nualsgi_n.h>
#pragma GCC diagnostic pop

#include <malloc.h>

char memory_heap[FW64_N64_HEAP_SIZE];

Audio audio;
Assets assets;
Input input;
Renderer renderer;
Time time;

int system_init(System* system) {
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
    assets_init(system->assets);
    audio_init(system->audio);

    filesystem_init();

    return 1;
}

void system_update(System* system) {
    time_update(system->time);
    input_update(system->input);
}