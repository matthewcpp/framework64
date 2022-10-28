#include "framework64/n64/texture.h"
#include "framework64/n64/image.h"

#include <stdlib.h>
#include <malloc.h>

fw64Texture* fw64_texture_create_from_image(fw64Image* image, fw64Allocator* allocator) {
    if (!allocator) allocator = fw64_default_allocator();
    fw64Texture* texture = allocator->malloc(allocator, sizeof(fw64Texture));

    fw64_n64_texture_init_with_image(texture, image);

    return texture;
}

void fw64_n64_texture_init_with_image(fw64Texture* texture, fw64Image* image) {
    texture->image = image;
    texture->wrap_s = G_TX_CLAMP;
    texture->wrap_t = G_TX_CLAMP;
    texture->mask_s = G_TX_NOMASK;
    texture->mask_t = G_TX_NOMASK;
}

fw64Image* fw64_texture_get_image(fw64Texture* texture) {
    return texture->image;
}

void fw64_texture_set_image(fw64Texture* texture, fw64Image* image) {
    texture->image = image;
}

void fw64_texture_delete(fw64Texture* texture, fw64Allocator* allocator) {
    if (!allocator) allocator = fw64_default_allocator();
    allocator->free(allocator, texture);
}

int fw64_texture_slice_width(fw64Texture* texture) {
    return texture->image->info.width / texture->image->info.hslices;
}

int fw64_texture_slice_height(fw64Texture* texture) {
    return texture->image->info.height / texture->image->info.vslices;
}

int fw64_texture_width(fw64Texture* texture) {
    return texture->image->info.width;
}

int fw64_texture_height(fw64Texture* texture) {
    return texture->image->info.height;
}

int fw64_texture_hslices(fw64Texture* texture) {
    return texture->image->info.hslices;
}

int fw64_texture_vslices(fw64Texture* texture) {
    return texture->image->info.vslices;
}

void fw64_texture_set_wrap_mode(fw64Texture* texture, fw64TextureWrapMode wrap_s, fw64TextureWrapMode wrap_t) {
    texture->wrap_s = wrap_s;
    texture->mask_s = (wrap_s == FW64_TEXTURE_WRAP_CLAMP) ? 0 : 4;

    texture->wrap_t = wrap_t;
    texture->mask_t = (wrap_t == FW64_TEXTURE_WRAP_CLAMP) ? 0 : 4;
}
