#pragma once

#include "framework64/asset_database.h"
#include "framework64/audio.h"
#include "framework64/input.h"
#include "framework64/renderer.h"
#include "framework64/time.h"

typedef struct {
    fw64AssetDatabase* assets;
    fw64Audio* audio;
    fw64Input* input;
    fw64Renderer* renderer;
    fw64Time* time;
} fw64Engine;
