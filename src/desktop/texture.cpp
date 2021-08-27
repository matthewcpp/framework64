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

    GLenum image_data_type = surface->format->BytesPerPixel == 4 ? GL_BGRA : GL_BGR;

    glGenTextures(1, &texture->gl_handle);
    glBindTexture(GL_TEXTURE_2D, texture->gl_handle);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->width, texture->height, 0, image_data_type, GL_UNSIGNED_BYTE, surface->pixels);

    glGenerateMipmap(GL_TEXTURE_2D);

    SDL_FreeSurface(surface);

    return texture;
}

int fw64_texture_get_slice_width(fw64Texture* texture) {
    return texture->width / texture->hslices;
}

int fw64_texture_get_slice_height(fw64Texture* texture) {
    return texture->height / texture->vslices;
}
