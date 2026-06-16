#include "${game_include_path}"
#include "framework64/n64_libultra/engine.h"

#include "assets/assets.h"

#include <nusys.h>

Game game __attribute__ ((aligned (8)));
fw64Engine engine;
float accumulated_time = 0.0f;

// In this case we will delay game execution until start is pressed.
// this gives time to attach the debugger and debug code in the game's init method
#ifdef FW64_N64LIBULTRA_WAIT_FOR_START
int wait_for_start = 1;
#endif

void nusys_game_tick(int pendingGfx) {
    if (pendingGfx < 1) {
        #ifdef FW64_N64LIBULTRA_WAIT_FOR_START
        if (wait_for_start) {
            NUContData current_state[NU_CONT_MAXCONTROLLERS];
            nuContDataGetExAll(current_state);

            if (current_state[0].trigger & START_BUTTON){
                wait_for_start = 0;
                game_init(&game, &engine);
            } else {
                return;
            }
        }   
        #endif

        fw64_n64_libultra_engine_update(&engine);

        accumulated_time += engine.time->time_delta;

        while (accumulated_time >= engine.time->fixed_time_delta) {
            game_fixed_update(&game);
            accumulated_time -= engine.time->fixed_time_delta;
        }

        engine.time->accumulator_progress = accumulated_time / engine.time->fixed_time_delta;

        game_update(&game);
        game_draw(&game);
        fw64_n64_libultra_engine_finalize_frame(&engine);
    }
}

void mainproc(void) {
    fw64_n64_libultra_engine_init(&engine, FW64_ASSET_COUNT);

#ifndef FW64_N64LIBULTRA_WAIT_FOR_START
    game_init(&game, &engine);
#endif
    
    nuGfxFuncSet((NUGfxFunc)nusys_game_tick);
    nuGfxDisplayOn();
}
