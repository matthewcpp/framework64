#ifndef SPRITE_H
#define SPRITE_H

#include "ultra/texture.h"
#include "ultra/vec2.h"

#include <stdint.h>

typedef struct {
    uint16_t width;
    uint16_t height;
    uint8_t hslices;
    uint8_t vslices;
    uint8_t slice_count;
    uint8_t flags;
    const uint8_t** slices;
} ImageSprite;

int image_sprite_get_slice_width(ImageSprite* sprite);
int image_sprite_get_slice_height(ImageSprite* sprite);

#endif
