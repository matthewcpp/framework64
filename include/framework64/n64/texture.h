#ifndef FW64_N64_TEXTURE_H
#define FW64_N64_TEXTURE_H

#include "framework64/texture.h"

#include <stdint.h>

struct fw64Texture {
    uint16_t width;
    uint16_t height;
    uint16_t hslices;
    uint16_t vslices;
    uint32_t wrap_s;
    uint32_t wrap_t;
    uint32_t mask_s;
    uint32_t mask_t;
    uint8_t* data;
};

int fw64_n64_texture_load(int assetIndex, fw64Texture* texture);
void fw64_n64_texture_uninit(fw64Texture* texture);

#endif