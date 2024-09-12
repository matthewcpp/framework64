#pragma once

#include "framework64/desktop/mesh.hpp"
#include "framework64/desktop/shader_cache.hpp"

#include <array>

namespace framework64 {
/// TODO: look into making this use a mesh?
class ScreenOverlay {
public:
    ScreenOverlay(ShaderCache& shader_cache)
        :material(shader_cache) {}
public:
    void init();

    fw64Primitive primitive;
    fw64Material material;
};

}