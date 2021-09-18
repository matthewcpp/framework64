#pragma once

#include "framework64/texture.h"
#include "framework64/image.h"

#include <stdint.h>

struct fw64Texture {
    fw64Image* image;
    uint32_t wrap_s;
    uint32_t wrap_t;
    uint32_t mask_s;
    uint32_t mask_t;
};

void fw64_n64_texture_init_with_image(fw64Texture* texture, fw64Image* image);
void fw64_n64_texture_uninit(fw64Texture* texture);
