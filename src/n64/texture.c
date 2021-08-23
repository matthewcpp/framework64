#include "framework64/texture.h"

#include "framework64/filesystem.h"

#include <nusys.h>

#include <stdlib.h>
#include <malloc.h>
#include "framework64/n64/texture.h"

int fw64_texture_get_slice_width(fw64Texture* sprite) {
    return sprite->width / sprite->hslices;
}

int fw64_texture_get_slice_height(fw64Texture* sprite) {
    return sprite->height / sprite->vslices;
}

int fw64_texture_load(int assetIndex, fw64Texture* sprite) {
    int handle = fw64_filesystem_open(assetIndex);
    if (handle < 0)
        return 0;

    
    int bytes_read = fw64_filesystem_read(sprite, sizeof(uint16_t), 4, handle);
    if (bytes_read != 8) {
        fw64_filesystem_close(handle);
        return 0;
    }

    int data_size = sprite->width * sprite->height * 2;
    sprite->data = memalign(8, data_size);
    bytes_read = fw64_filesystem_read(sprite->data, 1, data_size, handle);

    sprite->wrap_s = G_TX_CLAMP;
    sprite->wrap_t = G_TX_CLAMP;
    sprite->mask_s = G_TX_NOMASK;
    sprite->mask_t = G_TX_NOMASK;

    fw64_filesystem_close(handle);
 
    return bytes_read == data_size;
}

void fw64_texture_uninit(fw64Texture* sprite) {
    free(sprite->data);
}