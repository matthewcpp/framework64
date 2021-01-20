#include <nusys.h>

#include "arcball_example.h"
#include "ultra/input.h"
#include "ultra/renderer.h"

#include <malloc.h>

ArcballExample example;
Renderer renderer;
Input input;

void nusys_game_update(int pendingGfx);

#define HEAP_SIZE 1024*512*1

char memory_heap[HEAP_SIZE];

void mainproc(void) {
  InitHeap(memory_heap, HEAP_SIZE);

  renderer_init(&renderer, 320, 240);
  input_init(&input);

  arcball_example_init(&example, &renderer, &input);
  nuGfxFuncSet((NUGfxFunc)nusys_game_update);
  nuGfxDisplayOn();
}

void nusys_game_update(int pendingGfx) {
  if (pendingGfx < 1) {
    input_update(&input);
    arcball_example_update(&example, 0.0333);
    arcball_example_draw(&example);
  }
}
