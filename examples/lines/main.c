#include "game.h"
#include "framework64/system.h"

#include <nusys.h>

Game example;
fw64System system;

void nusys_game_tick(int pendingGfx) {
    if (pendingGfx < 1) {
        fw64_sytem_update(&system);
        game_update(&example, system.time->time_delta);
        game_draw(&example);
    }
}

void mainproc(void) {
    fw64_system_init(&system);
    game_init(&example, &system);
    nuGfxFuncSet((NUGfxFunc)nusys_game_tick);
    nuGfxDisplayOn();
}
