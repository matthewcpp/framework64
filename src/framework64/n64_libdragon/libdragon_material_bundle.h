#pragma once

#include "framework64/material_bundle.h"

#include "libdragon_material.h"
#include "libdragon_texture.h"
#include "libdragon_image.h"

struct fw64MaterialBundle {
    uint32_t material_count;
    uint32_t texture_count;
    uint32_t image_count;

    fw64Material* materials;
    fw64Texture* textures;
    fw64Image* images;
};
