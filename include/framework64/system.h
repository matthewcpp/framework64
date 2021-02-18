#ifndef FW64_SYSTEM_H
#define FW64_SYSTEM_H

#include "framework64/asset.h"
#include "framework64/input.h"
#include "framework64/renderer.h"
#include "framework64/time.h"

#define FW64_N64_SCREEN_WIDTH 320
#define FW64_N64_SCREEN_HEIGHT 240

#define FW64_N64_HEAP_SIZE 1024*512

typedef struct {
    Input* input;
    Renderer* renderer;
    Time* time;
    Assets* assets;
} System;

int fw64_system_init(System* system);
int fw64_sytem_update(System* system);

#endif