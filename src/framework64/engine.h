#pragma once

/** \file engine.h */

#include "framework64/asset_database.h"
#include "framework64/audio.h"
#include "framework64/display.h"
#include "framework64/input.h"
#include "framework64/modules.h"
#include "framework64/renderer.h"
#include "framework64/save_file.h"
#include "framework64/time.h"

typedef struct {
    fw64AssetDatabase* assets;
    fw64Audio* audio;
    fw64Displays* displays;
    fw64Input* input;
    fw64Renderer* renderer;
    fw64Modules* modules;
    fw64SaveFile* save_file;
    fw64Time* time;
} fw64Engine;
