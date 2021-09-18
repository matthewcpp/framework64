#pragma once

#include "framework64/texture.h"

#include <string>
#include <cstdint>

struct fw64Texture {
    int width;
    int height;
    int hslices;
    int vslices;
    uint32_t gl_handle;

    fw64TextureWrapMode wrap_s = FW64_TEXTURE_WRAP_CLAMP;
    fw64TextureWrapMode wrap_t = FW64_TEXTURE_WRAP_CLAMP;
    
    inline int slice_width() const { return width / hslices; }
    inline int slice_height() const { return height / vslices; }

    /** Load image file into OpenGL */
    static fw64Texture* loadImageFile(std::string const& path);
    static fw64Texture* loadImageBuffer(void* data, size_t size);
};
