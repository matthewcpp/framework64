#include "ultra/sprite.h"

int image_sprite_get_slice_width(ImageSprite* sprite) {
    return sprite->width / sprite->hslices;
}

int image_sprite_get_slice_height(ImageSprite* sprite) {
    return sprite->height / sprite->vslices;
}