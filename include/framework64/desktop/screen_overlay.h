#pragma once

#include "framework64/desktop/mesh.h"
#include "framework64/desktop/shader_cache.h"

#include <array>

namespace framework64 {
/// TODO: look into making this use a mesh?
class ScreenOverlay {
public:
    void init(ShaderCache& shader_cache);

    fw64Primitive primitive;
    fw64Material material;
};

}