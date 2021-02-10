#include "arcball_example.h"
#include "framework64/system.h"

#include <nusys.h>

ArcballExample example;
fw64System system;

void nusys_game_tick(int pendingGfx) {
    if (pendingGfx < 1) {
        fw64_sytem_update(&system);
        arcball_example_update(&example, system.time.time_delta);
        arcball_example_draw(&example);
    }
}

void mainproc(void) {
    fw64_system_init(&system);

    arcball_example_init(&example, &system.renderer, &system.input);
    nuGfxFuncSet((NUGfxFunc)nusys_game_tick);
    nuGfxDisplayOn();
}
