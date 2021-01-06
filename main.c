#include <nusys.h>

#include "game.h"

#include <malloc.h>

Game* game = NULL;
Renderer* renderer = NULL;

void nusys_game_update(int pendingGfx);

char memory_heap[1024*512*1];

void mainproc(void) {
  nuGfxInit();
  InitHeap(memory_heap, 1024*512*1);
  nuContInit();
  renderer = renderer_create(320, 240);
  game = game_create(renderer);
  nuGfxFuncSet((NUGfxFunc)nusys_game_update);
  nuGfxDisplayOn();
}

void nusys_game_update(int pendingGfx) {
  if (pendingGfx < 1) {
    game_update(game, 0.0f);
    game_draw(game);
  }
}
