#include <nusys.h>

#include "game.h"
#include "input.h"
#include "renderer.h"

#include <malloc.h>

Game* game = NULL;
Renderer* renderer = NULL;
Input* input = NULL;

void nusys_game_update(int pendingGfx);

#define HEAP_SIZE 1024*512*1

char memory_heap[HEAP_SIZE];

void mainproc(void) {
  nuGfxInit();
  InitHeap(memory_heap, HEAP_SIZE);
  nuContInit();

  renderer = renderer_create(320, 240);
  input  = input_create();

  game = game_create(renderer, input);
  nuGfxFuncSet((NUGfxFunc)nusys_game_update);
  nuGfxDisplayOn();
}

void nusys_game_update(int pendingGfx) {
  if (pendingGfx < 1) {
    input_update(input);
    game_update(game, 0.0333);
    game_draw(game);
  }
}
