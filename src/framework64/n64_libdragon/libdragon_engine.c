#include "libdragon_engine.h"

#include "libdragon_asset_database.h"
#include "libdragon_display.h"
#include "libdragon_input.h"
#include "libdragon_renderer.h"

#include <libdragon.h>
#include <GL/gl_integration.h>

#include <string.h>

fw64AssetDatabase asset_database;
fw64Displays displays;
fw64Input input;
fw64Renderer renderer;

void fw64_n64_libdragon_engine_init(fw64Engine* engine) {
    _fw64_default_allocator_init();
    fw64_n64_libdragon_displays_init(&displays);

#ifndef NDEBUG
    //Init logging
    debug_init_isviewer(); // logging in supported emulators writes to std::err
    //debug_init_usblog();
#endif

    //Init Graphics
    rdpq_init();
    gl_init();
    fw64_libdragon_renderer_init(&renderer);

    //Init display
    display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE);

    //Init DragonFS
    dfs_init(DFS_DEFAULT_LOCATION);

    //Init joypad
    joypad_init();
    memset(&input, 0, sizeof(input));

    engine->assets = &asset_database;
    engine->displays = &displays;
    engine->input = &input;
    engine->renderer = &renderer;
}

void fw64_n64_libdragon_engine_update(fw64Engine* engine) {
    joypad_poll();

    fw64_n64_libdragon_input_update(&input);
}