#include "libdragon_engine.h"

#include <libdragon.h>
#include <GL/gl_integration.h>

#include <string.h>

void fw64_n64_libdragon_engine_init(fw64LibdragonEngine* engine) {
    _fw64_default_allocator_init();
    fw64_n64_libdragon_displays_init(&engine->displays);

    engine->time.total_time = 0.0f;

#ifndef NDEBUG
    //Init logging
    debug_init_isviewer(); // logging in supported emulators writes to std::err
    //debug_init_usblog();
#endif

    //Init Graphics
    rdpq_init();
    yuv_init(); // only needed if enabling FMV?
    gl_init();
    fw64_libdragon_renderer_init(&engine->renderer);

    //Init display
    //display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE);
    #define NUM_DISPLAY_BUFFERS 3
    display_init(RESOLUTION_320x240, DEPTH_16_BPP, NUM_DISPLAY_BUFFERS, GAMMA_NONE, FILTERS_RESAMPLE_ANTIALIAS_DEDITHER);

    //Init DragonFS
    dfs_init(DFS_DEFAULT_LOCATION);
    fw64_libdragon_asset_database_init(&engine->asset_database);

    //Init joypad
    joypad_init();
    memset(&engine->input, 0, sizeof(fw64Input));

    //Init Audio
    // Needs to be 48 kHz if Opus audio compression is used.
    // TODO: This can be conditionally set if video support enabled?
    //#define AUDIO_HZ 32000.0f
    #define AUDIO_HZ 48000.0f
    audio_init(AUDIO_HZ, 4);
    mixer_init(8);

    fw64_libdragon_fmv_init(&engine->fmv, &engine->displays);

    engine->interface.assets = &engine->asset_database;
    engine->interface.audio = &engine->audio;
    engine->interface.displays = &engine->displays;
    engine->interface.fmv = &engine->fmv;
    engine->interface.input = &engine->input;
    engine->interface.renderer = &engine->renderer;
    engine->interface.time = &engine->time;
}

void fw64_n64_libdragon_engine_update(fw64LibdragonEngine* engine) {
    joypad_poll();

    fw64_n64_libdragon_input_update(&engine->input);

    // TODO: actual implementation needed
    engine->time.time_delta = 1.0f / 45.0f;
    engine->time.total_time += engine->time.time_delta;

    fw64_libdragon_fmv_update(&engine->fmv);
    if (engine->fmv.status != FW64_FMV_CLOSED) {
        mixer_throttle(AUDIO_HZ / engine->displays.fps);
    }
}

void fw64_n64_libdragon_engine_draw(fw64LibdragonEngine* engine) {
    mixer_try_play();
    fw64_libdragon_fmv_draw(&engine->fmv);
    mixer_try_play();
}
