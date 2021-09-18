#include "framework64/n64/texture.h"

#include "framework64/filesystem.h"

#include <nusys.h>

#include <stdlib.h>
#include <malloc.h>

int fw64_texture_slice_width(fw64Texture* texture) {
    return texture->width / texture->hslices;
}

int fw64_texture_slice_height(fw64Texture* texture) {
    return texture->height / texture->vslices;
}

int fw64_texture_width(fw64Texture* texture) {
    return texture->width;
}

int fw64_texture_height(fw64Texture* texture) {
    return texture->height;
}

int fw64_texture_hslices(fw64Texture* texture) {
    return texture->hslices;
}

int fw64_texture_vslices(fw64Texture* texture) {
    return texture->vslices;
}

int fw64_n64_texture_load(int assetIndex, fw64Texture* texture) {
    int handle = fw64_filesystem_open(assetIndex);
    if (handle < 0)
        return 0;

    
    int bytes_read = fw64_filesystem_read(texture, sizeof(uint16_t), 4, handle);
    if (bytes_read != 8) {
        fw64_filesystem_close(handle);
        return 0;
    }

    int data_size = texture->width * texture->height * 2;
    texture->data = memalign(8, data_size);
    bytes_read = fw64_filesystem_read(texture->data, 1, data_size, handle);

    texture->wrap_s = G_TX_CLAMP;
    texture->wrap_t = G_TX_CLAMP;
    texture->mask_s = G_TX_NOMASK;
    texture->mask_t = G_TX_NOMASK;

    fw64_filesystem_close(handle);
 
    return bytes_read == data_size;
}

void fw64_texture_set_wrap_mode(fw64Texture* texture, fw64TextureWrapMode wrap_s, fw64TextureWrapMode wrap_t) {
    texture->wrap_s = wrap_s;
    texture->mask_s = wrap_s != FW64_TEXTURE_WRAP_CLAMP ? 4 : 0;
    texture->wrap_t = wrap_t;
    texture->mask_t = wrap_t != FW64_TEXTURE_WRAP_CLAMP ? 4 : 0;
}

void fw64_n64_texture_uninit(fw64Texture* texture) {
    free(texture->data);
}
