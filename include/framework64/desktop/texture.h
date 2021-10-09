#pragma once

#include "framework64/desktop/image.h"
#include "framework64/texture.h"

#include <string>
#include <cstdint>

struct fw64Texture {
    fw64Texture(fw64Image* img);

    fw64Image* image;
    fw64TextureWrapMode wrap_s = FW64_TEXTURE_WRAP_CLAMP;
    fw64TextureWrapMode wrap_t = FW64_TEXTURE_WRAP_CLAMP;
    
    inline int slice_width() const { return image->width / image->hslices; }
    inline int slice_height() const { return image->height / image->vslices; }
};
