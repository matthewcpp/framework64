#pragma once

#include "framework64/material.h"
#include "framework64/texture.h"

#include <array>
#include <cstdint>
#include <limits>
#include <memory>

namespace framework64 {
struct ShaderProgram;
class ShaderCache;
}

#define FW64_DESKTOP_ENTIRE_TEXTURE_FRAME (std::numeric_limits<uint32_t>::max())

struct fw64Material {
    fw64Material(framework64::ShaderCache& shader_cache)
        :shader_cache(shader_cache) {}

    enum Features {
        None = 0,
        DiffuseTexture = 1
    };

    std::array<float, 4> color = {1.0f, 1.0f, 1.0f, 1.0f};
    fw64ShadingMode shading_mode = FW64_SHADING_MODE_UNSET;
    
    fw64Texture* texture = nullptr;
    uint32_t texture_frame = 0;
    framework64::ShaderProgram* shader = nullptr;

    [[nodiscard]] uint32_t featureMask() const {
        return texture != nullptr ? Features::DiffuseTexture : Features::None;
    }

    framework64::ShaderCache& shader_cache;

    void updateShader();
};
