#include "framework64/sprite.h"

#include "framework64/filesystem.h"

#include <stdlib.h>
#include <malloc.h>

int image_sprite_get_slice_width(ImageSprite* sprite) {
    return sprite->width / sprite->hslices;
}

int image_sprite_get_slice_height(ImageSprite* sprite) {
    return sprite->height / sprite->vslices;
}

int sprite_load(int assetIndex, ImageSprite* sprite) {
    int handle = filesystem_open(assetIndex);
    if (handle < 0)
        return 0;

    
    int bytes_read = filesystem_read(sprite, sizeof(uint16_t), 4, handle);
    if (bytes_read != 8) {
        filesystem_close(handle);
        return 0;
    }

    int data_size = sprite->width * sprite->height * 2;
    sprite->data = memalign(8, data_size);
    bytes_read = filesystem_read(sprite->data, 1, data_size, handle);

    filesystem_close(handle);
 
    return bytes_read == data_size;
}

void sprite_uninit(ImageSprite* sprite) {
    free(sprite->data);
}