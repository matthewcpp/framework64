#include "framework64/system.h"

#include "framework64/filesystem.h"

#include <nusys.h>

#include <malloc.h>

char memory_heap[FW64_N64_HEAP_SIZE];

int fw64_system_init(fw64System* system) {
    InitHeap(memory_heap, FW64_N64_HEAP_SIZE);

    nuGfxInit(); // starts nusys graphics

    renderer_init(&system->renderer, 320, 240);
    input_init(&system->input);
    time_init(&system->time);

    filesystem_init();

    return 1;
}

int fw64_sytem_update(fw64System* system) {
    time_update(&system->time);
    input_update(&system->input);
}