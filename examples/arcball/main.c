#include <nusys.h>

#include "arcball_example.h"
#include "framework64/input.h"
#include "framework64/renderer.h"
#include "framework64/time.h"

#include <malloc.h>

ArcballExample example;
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

  arcball_example_init(&example, &renderer, &input);
  nuGfxFuncSet((NUGfxFunc)nusys_game_update);
  nuGfxDisplayOn();
}

void nusys_game_update(int pendingGfx) {
  if (pendingGfx < 1) {
    time_update(&time);
    input_update(&input);
    arcball_example_update(&example, time.time_delta);
    arcball_example_draw(&example);
  }
}
