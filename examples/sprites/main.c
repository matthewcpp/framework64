#include "sprites_example.h"
#include "framework64/system.h"

#include <nusys.h>

SpritesExample example;
fw64System system;

void nusys_game_tick(int pendingGfx) {
    if (pendingGfx < 1) {
        fw64_sytem_update(&system);
        sprites_example_update(&example, system.time.time_delta);
        sprites_example_draw(&example);
    }
}

void mainproc(void) {
    fw64_system_init(&system);

    sprites_example_init(&example, &system.input, &system.renderer);
    nuGfxFuncSet((NUGfxFunc)nusys_game_tick);
    nuGfxDisplayOn();
}
