#include "game.h"
#include "framework64/engine.h"

#include <nusys.h>

Game game;
fw64Engine engine;

void nusys_game_tick(int pendingGfx) {
    if (pendingGfx < 1) {
        fw64_engine_update(&engine);
        game_update(&game, engine.time->time_delta);
        game_draw(&game);
    }
}

void mainproc(void) {
    fw64_engine_init(&engine);
    game_init(&game, &engine);
    nuGfxFuncSet((NUGfxFunc)nusys_game_tick);
    nuGfxDisplayOn();
}
