#include "data_example.h"
#include "framework64/system.h"

#include <nusys.h>

DataExample example;
fw64System system;

void nusys_game_tick(int pendingGfx) {
    if (pendingGfx < 1) {
        fw64_sytem_update(&system);
        data_example_update(&example, system.time.time_delta);
        data_example_draw(&example);
    }
}

void mainproc(void) {
    fw64_system_init(&system);

    data_example_init(&example, &system.input, &system.renderer);
    nuGfxFuncSet((NUGfxFunc)nusys_game_tick);
    nuGfxDisplayOn();
}
