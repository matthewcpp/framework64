#pragma once

#include "framework64/desktop/uniform_block.hpp"
#include "framework64/desktop/material.hpp"
#include "framework64/desktop/texture.hpp"

namespace framework64 {

class TextureInfoUniform {
public:
    struct TextureInfoData{
        float slice_top;
        float slice_left;
        float slice_width;
        float slice_height;
    };

    UniformBlock<TextureInfoData> uniform_block;

    void setUniformData(fw64Material const & material) {
        if (material.texture_frame == FW64_DESKTOP_ENTIRE_TEXTURE_FRAME) {
            uniform_block.data.slice_top = 0.0f;
            uniform_block.data.slice_left = 0.0f;
            uniform_block.data.slice_width = 1.0f;
            uniform_block.data.slice_height = 1.0f;
        }
        else {
            auto* image = material.texture->image;

            auto row = static_cast<float>(material.texture_frame / image->hslices);
            auto col = static_cast<float>(material.texture_frame % image->hslices);

            uniform_block.data.slice_top = row / static_cast<float>(image->vslices);
            uniform_block.data.slice_left = col / static_cast<float>(image->hslices);
            uniform_block.data.slice_width = static_cast<float>(material.texture->slice_width()) / static_cast<float>(image->width);
            uniform_block.data.slice_height = static_cast<float>(material.texture->slice_height()) / static_cast<float>(image->height);
        }
    }
};

}