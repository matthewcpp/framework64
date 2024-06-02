#pragma once

#include "framework64/color.h"
#include "framework64/sprite_batch.h"

#include "framework64/n64_libultra/texture_batch.h"

#include <nusys.h>

struct fw64SpriteBatch {
    fw64ColorRGBA8 color;
    fw64N64TextureBatch texture_batch;
};