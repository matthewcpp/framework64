#include "game.h"
#include "framework64/n64_libultra/engine.h"

#include "assets/assets.h"

#include <nusys.h>

Game game __attribute__ ((aligned (8)));;
fw64Engine engine;

void nusys_game_tick(int pendingGfx) {
    if (pendingGfx < 1) {
        fw64_n64_engine_update(&engine);
        game_update(&game);
        game_draw(&game);
    }
}

void mainproc(void) {
    fw64_n64_engine_init(&engine, FW64_ASSET_COUNT);
    game_init(&game, &engine);
    fw64_n64_engine_post_game_init(&engine);
    nuGfxFuncSet((NUGfxFunc)nusys_game_tick);
    nuGfxDisplayOn();
}
