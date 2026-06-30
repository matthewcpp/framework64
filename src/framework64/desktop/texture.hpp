#pragma once

#include "framework64/texture.h"
#include "framework64/desktop/image.hpp"
#include "framework64/desktop/openGL.hpp"
#include <cstdint>
#include <string>

struct fw64Texture {
    fw64Texture();
    fw64Texture(fw64Image* img);

    void setWrapMode(fw64TextureWrapMode s, fw64TextureWrapMode t);

    fw64Image* image = nullptr;
    GLenum wrap_s = FW64_TEXTURE_WRAP_CLAMP;
    GLenum wrap_t = FW64_TEXTURE_WRAP_CLAMP;
    uint32_t palette_index = 0;
    
    inline int slice_width() const { return image->width / image->hslices; }
    inline int slice_height() const { return image->height / image->vslices; }
    GLuint getGlImageHandle() const;
};
