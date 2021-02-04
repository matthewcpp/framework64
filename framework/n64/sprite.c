#include "framework64/sprite.h"

#include <stdlib.h>
#include <malloc.h>

int image_sprite_get_slice_width(ImageSprite* sprite) {
    return sprite->width / sprite->hslices;
}

int image_sprite_get_slice_height(ImageSprite* sprite) {
    return sprite->height / sprite->vslices;
}

void image_sprite_uninit(ImageSprite* sprite) {
    if (sprite->flags & SPRITE_FLAG_DYNAMIC) {
        for (uint8_t i = 0; i < sprite->slice_count; i++) {
            free(sprite->slices[i]);
        }

        free(sprite->slices);
    }
}