#pragma once

#include "framework64/allocator.h"
#include "framework64/data_io.h"

#include "framework64/material_bundle.h"

#include "framework64/desktop/image.hpp"
#include "framework64/desktop/material.hpp"
#include "framework64/desktop/texture.hpp"

#include <vector>
#include <memory>

namespace framework64 {
    class ShaderCache;
}

struct fw64MaterialBundle {
    std::vector<std::unique_ptr<fw64Image>> images;
    std::vector<std::unique_ptr<fw64Texture>> textures;
    std::vector<std::unique_ptr<fw64Material>> materials;

    static fw64MaterialBundle* loadFromDatasource(fw64DataSource* data_source, framework64::ShaderCache& shader_cache, fw64Allocator* allocator); 
};

