#include <nusys.h>

#include "lines_example.h"
#include "framework64/input.h"
#include "framework64/renderer.h"
#include "framework64/time.h"

#include <malloc.h>

LinesExample example;
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

  lines_example_init(&example, &input, &renderer);
  nuGfxFuncSet((NUGfxFunc)nusys_game_update);
  nuGfxDisplayOn();
}

void nusys_game_update(int pendingGfx) {
  if (pendingGfx < 1) {
    time_update(&time);
    input_update(&input);
    lines_example_update(&example, time.time_delta);
    lines_example_draw(&example);
  }
}
