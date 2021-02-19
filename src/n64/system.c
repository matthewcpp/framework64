#include "framework64/system.h"

#include "framework64/filesystem.h"

#include <nusys.h>

#include <malloc.h>

char memory_heap[FW64_N64_HEAP_SIZE];

Assets assets;
Input input;
Renderer renderer;
Time time;

int fw64_system_init(System* system) {
    system->assets = &assets;
    system->input = &input;
    system->renderer = &renderer;
    system->time = &time;

    InitHeap(memory_heap, FW64_N64_HEAP_SIZE);

    nuGfxInit(); // starts nusys graphics

    renderer_init(system->renderer, FW64_N64_SCREEN_WIDTH, FW64_N64_SCREEN_HEIGHT);
    input_init(system->input);
    time_init(system->time);
    assets_init(system->assets);

    filesystem_init();

    return 1;
}

void fw64_sytem_update(System* system) {
    time_update(system->time);
    input_update(system->input);
}