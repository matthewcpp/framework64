#pragma once

#include "framework64/material.h"
#include "framework64/texture.h"

#include <array>
#include <cstdint>
#include <memory>

namespace framework64 {
class ShaderProgram;
}

#define FW64_DESKTOP_ENTIRE_TEXTURE_FRAME (-1)

struct fw64Material {
    enum Features {
        None = 0,
        DiffuseTexture = 1
    };

    std::array<float, 4> color = {1.0f, 1.0f, 1.0f, 1.0f};
    fw64ShadingMode shading_mode = FW64_SHADING_MODE_UNSET;
    
    fw64Texture* texture = nullptr;
    int texture_frame = 0;
    framework64::ShaderProgram* shader = nullptr;

    [[nodiscard]] uint32_t featureMask() const {
        return texture != nullptr ? Features::DiffuseTexture : Features::None;
    }

    void updateShader();
};
