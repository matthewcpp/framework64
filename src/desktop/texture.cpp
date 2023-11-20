#include "framework64/desktop/texture.h"

#include <SDL2/SDL_image.h>

#include <cassert>
#include <utility>

fw64Texture::fw64Texture(fw64Image* img) {
    image = img;
}

GLuint fw64Texture::getGlImageHandle() const {
    if (image->palettes.empty()) {
        return image->gl_handle;
    }
    else {
        return image->palettes[palette_index];
    }
}

// C Interface

fw64Texture* fw64_texture_create_from_image(fw64Image* image, fw64Allocator* allocator) {
    // TODO: fix this...
    assert(allocator != nullptr);
    return new fw64Texture(image);
}

void fw64_texture_delete(fw64Texture* texture, fw64Allocator* allocator) {
    assert(allocator != nullptr);
    delete (texture);
}

fw64Image* fw64_texture_get_image(fw64Texture* texture) {
    return texture->image;
}

void fw64_texture_set_image(fw64Texture* texture, fw64Image* image) {
    texture->image = image;
    texture->palette_index = 0;
}

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

void fw64_texture_set_palette_index(fw64Texture* texture, uint32_t index) {
    texture->palette_index = index;
}

uint32_t fw64_texture_get_palette_index(fw64Texture* texture) {
    return texture->palette_index;
}
