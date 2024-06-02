#include "framework64/engine.h"

#include "framework64/allocator.h"
#include "framework64/n64_libultra/audio.h"
#include "framework64/n64_libultra/asset_database.h"
#include "framework64/n64_libultra/data_link.h"
#include "framework64/n64_libultra/display.h"
#include "framework64/n64_libultra/media.h"
#include "framework64/n64_libultra/input.h"
#include "framework64/n64_libultra/renderer.h"
#include "framework64/n64_libultra/save_file.h"

#include "framework64/n64_libultra/filesystem.h"

#include <nusys.h>
#include <ultra64.h>

#pragma GCC diagnostic ignored "-Wcomment"
#include <nualsgi_n.h>
#pragma GCC diagnostic pop

#include <malloc.h>
#include <string.h>

#define FW64_N64_SCREEN_WIDTH 320
#define FW64_N64_SCREEN_HEIGHT 240

#define FW64_N64_HEAP_SIZE 1024*512

char memory_heap[FW64_N64_HEAP_SIZE];

fw64Audio audio;
fw64AssetDatabase assets;
fw64Displays displays;
fw64Input input;
fw64Renderer renderer;
fw64Time time;
fw64DataLink data_link;
fw64SaveFile save_file;
fw64Media media;
u64 _previous_update_time = 0;

static void fw64_n64_engine_update_time(fw64Engine* engine) {
    u64 current_ms = OS_CYCLES_TO_USEC(osGetTime()) / 1000;

    if (current_ms < _previous_update_time)
        return;

    float ms_delta = (float)(current_ms - _previous_update_time);
    engine->time->time_delta = ms_delta / 1000.0f;
    engine->time->total_time += engine->time->time_delta;

    _previous_update_time = current_ms;
}

int fw64_n64_engine_init(fw64Engine* engine, int asset_count) {
    engine->audio = &audio;
    engine->assets = &assets;
    engine->data_link = &data_link;
    engine->displays = &displays;
    engine->input = &input;
    engine->media = &media;
    engine->renderer = &renderer;
    engine->save_file = &save_file;
    engine->time = &time;

    fw64_default_allocator_init();
    fw64_n64_data_link_init(engine->data_link);

    InitHeap(memory_heap, FW64_N64_HEAP_SIZE);

    nuGfxInit(); // starts nusys graphics
    nuAuInit(); //starts the SGI tools audio manager
    nuContInit(); // initialize nusys controller subsystem
    nuContRmbMgrInit();
    nuEepromMgrInit();

    fw64_n64_asset_database_init(engine->assets);
    fw64_n64_renderer_init(engine->renderer, FW64_N64_SCREEN_WIDTH, FW64_N64_SCREEN_HEIGHT);
    fw64_n64_libultra_displays_init(engine->displays, FW64_N64_SCREEN_WIDTH, FW64_N64_SCREEN_HEIGHT);
    fw64_n64_input_init(engine->input, engine->time);
    memset(engine->time, 0, sizeof(fw64Time));
    fw64_n64_audio_init(engine->audio);
    fw64_n64_save_file_init(engine->save_file);
    fw64_n64_media_init(engine->media);

    fw64_n64_filesystem_init(asset_count);

    return 1;
}

void fw64_n64_engine_post_game_init(fw64Engine* engine) {
    fw64_n64_data_link_start(engine->data_link);
}

void fw64_n64_engine_update(fw64Engine* engine) {
    fw64_n64_engine_update_time(engine);
    fw64_n64_input_update(engine->input);
    fw64_n64_audio_update(engine->audio);
    fw64_n64_data_link_update(engine->data_link);

    engine->renderer->starting_new_frame = 1;
}


#include <stdarg.h>
#include <ultra64.h> 


#ifndef NDEBUG
extern int _Printf(void *(*copyfunc)(void *, const char *, size_t), void*, const char*, va_list);
static void* printf_handler(void *buf, const char *str, size_t len)
{
    return ((char *) memcpy(buf, str, len) + len);
}

void fw64_debug_log(const char* message, ...) {
    char temp[128];
    va_list args;
    
    va_start(args, message);
    int len = _Printf(printf_handler, temp, message, args);
    va_end(args);

    fw64_data_link_send_message(&data_link, 1, &temp[0], len);
}

#endif