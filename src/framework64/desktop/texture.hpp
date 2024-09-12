#pragma once

#include "framework64/texture.h"
#include "framework64/desktop/image.hpp"
#include <cstdint>
#include <string>

struct fw64Texture {
    fw64Texture() = default;
    fw64Texture(fw64Image* img);

    fw64Image* image = nullptr;
    fw64TextureWrapMode wrap_s = FW64_TEXTURE_WRAP_CLAMP;
    fw64TextureWrapMode wrap_t = FW64_TEXTURE_WRAP_CLAMP;
    uint32_t palette_index = 0;
    
    inline int slice_width() const { return image->width / image->hslices; }
    inline int slice_height() const { return image->height / image->vslices; }
    GLuint getGlImageHandle() const;
};
