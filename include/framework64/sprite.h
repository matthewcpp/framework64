#ifndef SPRITE_H
#define SPRITE_H

#include "framework64/texture.h"
#include "framework64/vec2.h"

#include <stdint.h>

#define SPRITE_FLAG_DYNAMIC 1

typedef struct {
    uint16_t width;
    uint16_t height;
    uint16_t hslices;
    uint16_t vslices;
    uint8_t* data;
} ImageSprite;

int image_sprite_get_slice_width(ImageSprite* sprite);
int image_sprite_get_slice_height(ImageSprite* sprite);

int sprite_load(int assetIndex, ImageSprite* sprite);
void sprite_uninit(ImageSprite* sprite);

#endif
