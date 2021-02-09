#include <nusys.h>

#include "data_example.h"
#include "assets.h"

#include "framework64/filesystem.h"
#include "framework64/input.h"
#include "framework64/renderer.h"
#include "framework64/time.h"

#include <malloc.h>

DataExample example;
Renderer renderer;
Input input;
Time time;

void nusys_game_update(int pendingGfx);

#define HEAP_SIZE 1024*512*1

char memory_heap[HEAP_SIZE];

void mainproc(void) {
    InitHeap(memory_heap, HEAP_SIZE);

    renderer_init(&renderer, 320, 240);
    input_init(&input);
    time_init(&time);

    filesystem_init();

    data_example_init(&example, &input, &renderer);
    nuGfxFuncSet((NUGfxFunc)nusys_game_update);
    nuGfxDisplayOn();
}

void nusys_game_update(int pendingGfx) {
    if (pendingGfx < 1) {
        time_update(&time);
        input_update(&input);
        data_example_update(&example, time.time_delta);
        data_example_draw(&example);
    }
}
