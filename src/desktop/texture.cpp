#include "framework64/texture.h"
#include "framework64/desktop/texture.h"

#include <SDL2/SDL_image.h>

fw64Texture::fw64Texture(fw64Image* img) {
    image = img;
}

// C Interface

int fw64_texture_slice_width(fw64Texture* texture) {
    return texture->slice_width();
}

int fw64_texture_slice_height(fw64Texture* texture) {
    return texture->slice_height();
}

int fw64_texture_width(fw64Texture* texture) {
    return texture->image->width;
}

int fw64_texture_height(fw64Texture* texture) {
    return texture->image->height;
}

int fw64_texture_hslices(fw64Texture* texture) {
    return texture->image->hslices;
}

int fw64_texture_vslices(fw64Texture* texture) {
    return texture->image->vslices;
}

void fw64_texture_set_wrap_mode(fw64Texture* texture, fw64TextureWrapMode wrap_s, fw64TextureWrapMode wrap_t) {
    texture->wrap_s = wrap_s;
    texture->wrap_t = wrap_t;
}

void fw64_texture_delete(fw64Texture* texture) {
    delete (texture);
}