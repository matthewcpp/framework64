#pragma once

#include "framework64/desktop/mesh.h"
#include "framework64/desktop/shader_cache.h"

#include <array>

namespace framework64 {
class ScreenOverlay {
public:
    void init(ShaderCache& shader_cache);

    fw64Primitive primitive;
};

}