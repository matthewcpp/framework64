#include "${game_include_path}"
#include "framework64/n64_libultra/engine.h"

#include "assets/assets.h"

#include <nusys.h>

Game game __attribute__ ((aligned (8)));
fw64Engine engine;
float accumulated_time = 0.0f;

void nusys_game_tick(int pendingGfx) {
    if (pendingGfx < 1) {
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
    game_init(&game, &engine);
    nuGfxFuncSet((NUGfxFunc)nusys_game_tick);
    nuGfxDisplayOn();
}
