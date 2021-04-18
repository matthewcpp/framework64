#ifndef FW64_SPRITE_H
#define FW64_SPRITE_H

#include "framework64/vec2.h"

#include <stdint.h>

#define SPRITE_FLAG_DYNAMIC 1

typedef struct {
    uint16_t width;
    uint16_t height;
    uint16_t hslices;
    uint16_t vslices;
    uint32_t wrap_s;
    uint32_t wrap_t;
    uint32_t mask_s;
    uint32_t mask_t;
    uint8_t* data;
} fw64Texture;

int fw64_texture_get_slice_width(fw64Texture* sprite);
int fw64_texture_get_slice_height(fw64Texture* sprite);

int fw64_texture_load(int assetIndex, fw64Texture* sprite);
void fw64_texture_uninit(fw64Texture* sprite);

#endif
