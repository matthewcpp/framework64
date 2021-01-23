#include <nusys.h>

#include "sprites_example.h"
#include "ultra/input.h"
#include "ultra/renderer.h"
#include "ultra/time.h"

#include <malloc.h>

SpritesExample example;
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

  sprites_example_init(&example, &input, &renderer);
  nuGfxFuncSet((NUGfxFunc)nusys_game_update);
  nuGfxDisplayOn();
}

void nusys_game_update(int pendingGfx) {
  if (pendingGfx < 1) {
    time_update(&time);
    input_update(&input);
    sprites_example_update(&example, time.time_delta);
    sprites_example_draw(&example);
  }
}
