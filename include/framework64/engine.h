#pragma once

/** \file engine.h */

#include "framework64/asset_database.h"
#include "framework64/audio.h"
#include "framework64/data_link.h"
#include "framework64/input.h"
#include "framework64/media.h"
#include "framework64/renderer.h"
#include "framework64/save_file.h"
#include "framework64/time.h"

typedef struct {
    fw64AssetDatabase* assets;
    fw64Audio* audio;
    fw64DataLink* data_link;
    fw64Input* input;
    fw64Media* media;
    fw64Renderer* renderer;
    fw64SaveFile* save_file;
    fw64Time* time;
} fw64Engine;
