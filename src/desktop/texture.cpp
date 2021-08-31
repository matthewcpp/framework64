#include "framework64/texture.h"
#include "framework64/desktop/texture.h"

#include <gl/glew.h>
#include <SDL_image.h>

fw64Texture* fw64Texture::loadImageFile(std::string const& path) {
    SDL_Surface* surface = IMG_Load(path.c_str());

    if (!surface) {
        return nullptr;
    }

    auto* texture = new fw64Texture();

    texture->width = surface->w;
    texture->height = surface->h;
    texture->hslices = 1;
    texture->vslices = 1;

    GLenum textureFormat;
    if ((surface->format->Rmask & 0xFF) == 0xFF) { // RGB
        textureFormat = surface->format->BytesPerPixel == 4 ? GL_RGBA : GL_RGB;
    }
    else {
        textureFormat = surface->format->BytesPerPixel == 4 ? GL_BGRA : GL_BGR;
    }

    glGenTextures(1, &texture->gl_handle);
    glBindTexture(GL_TEXTURE_2D, texture->gl_handle);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->width, texture->height, 0, textureFormat, GL_UNSIGNED_BYTE, surface->pixels);

    glGenerateMipmap(GL_TEXTURE_2D);

    SDL_FreeSurface(surface);

    return texture;
}

int fw64_texture_slice_width(fw64Texture* texture) {
    return texture->slice_width();
}

int fw64_texture_slice_height(fw64Texture* texture) {
    return texture->slice_height();
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