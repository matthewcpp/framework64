#include "framework64/system.h"

#include "framework64/filesystem.h"

#include <nusys.h>

#include <malloc.h>

char memory_heap[FW64_N64_HEAP_SIZE];

Input input;
Renderer renderer;
Time time;

int fw64_system_init(fw64System* system) {
    system->input = &input;
    system->renderer = &renderer;
    system->time = &time;

    InitHeap(memory_heap, FW64_N64_HEAP_SIZE);

    nuGfxInit(); // starts nusys graphics

    renderer_init(system->renderer, FW64_N64_SCREEN_WIDTH, FW64_N64_SCREEN_HEIGHT);
    input_init(system->input);
    time_init(system->time);

    filesystem_init();

    return 1;
}

int fw64_sytem_update(fw64System* system) {
    time_update(system->time);
    input_update(system->input);
}