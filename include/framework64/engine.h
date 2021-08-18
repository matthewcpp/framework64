#ifndef FW64_SYSTEM_H
#define FW64_SYSTEM_H

#include "framework64/assets.h"
#include "framework64/audio.h"
#include "framework64/input.h"
#include "framework64/renderer.h"
#include "framework64/time.h"

typedef struct {
    fw64Audio* audio;
    fw64Input* input;
    fw64Renderer* renderer;
    fw64Time* time;
    fw64Assets* assets;
} fw64Engine;

int fw64_engine_init(fw64Engine* system);
void fw64_engine_update(fw64Engine* system);

#endif