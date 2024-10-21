#pragma once

#include "framework64/material_bundle.h"

#include "libdragon_material.h"
#include "libdragon_texture.h"
#include "libdragon_image.h"

typedef struct {
    uint32_t image_count;
    uint32_t texture_count;
    uint32_t material_count;
} fw64MaterialBundleInfo;

struct fw64MaterialBundle {
    fw64MaterialBundleInfo info;

    fw64Material* materials;
    fw64Texture* textures;
    fw64Image* images;
};
