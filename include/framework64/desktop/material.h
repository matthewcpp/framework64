#pragma once

#include "framework64/texture.h"

#include <array>

namespace framework64 {

class ShaderProgram;

struct Material {
    enum Features {
        None = 0,
        DiffuseTexture = 1
    };

    std::array<float, 4> color = {1.0f, 1.0f, 1.0f, 1.0f};
    fw64Texture* texture = nullptr;
    ShaderProgram* shader = nullptr;

    [[nodiscard]] uint32_t featureMask() const {
        return texture != nullptr ? Features::DiffuseTexture : Features::None;
    }
};

}