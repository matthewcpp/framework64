#ifndef FW64_SYSTEM_H
#define FW64_SYSTEM_H

#include "framework64/asset.h"
#include "framework64/audio.h"
#include "framework64/input.h"
#include "framework64/renderer.h"
#include "framework64/time.h"

#define FW64_N64_SCREEN_WIDTH 320
#define FW64_N64_SCREEN_HEIGHT 240

#define FW64_N64_HEAP_SIZE 1024*512

typedef struct {
    Audio* audio;
    Input* input;
    Renderer* renderer;
    Time* time;
    fw64Assets* assets;
} fw64Engine;

int fw64_engine_init(fw64Engine* system);
void fw64_engine_update(fw64Engine* system);

#endif