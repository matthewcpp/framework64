#pragma once

#include <array>

namespace framework64 {

class ShaderProgram;

struct Material {
    enum Features {
        None = 0
    };

    std::array<float, 4> color = {1.0f, 1.0f, 1.0f, 1.0f};
    ShaderProgram* shader = nullptr;

    uint32_t featureMask() const { return Features::None; }
};

}