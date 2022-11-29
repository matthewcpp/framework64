#pragma once

#include "framework64/texture.h"
#include "framework64/image.h"

#include <limits.h>
#include <stdint.h>

#define FW64_INVALID_PALETTE_INDEX UINT32_MAX

struct fw64Texture {
    fw64Image* image;
    uint32_t wrap_s;
    uint32_t wrap_t;
    uint32_t mask_s;
    uint32_t mask_t;
    uint32_t palette_index;
};

void fw64_n64_texture_init_with_image(fw64Texture* texture, fw64Image* image);
void fw64_n64_texture_uninit(fw64Texture* texture);
