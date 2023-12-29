#pragma once

#include "framework64/material_bundle.h"

#include "framework64/n64_libdragon/image.h"
#include "framework64/n64_libdragon/texture.h"
#include "framework64/n64_libdragon/material.h"

// this needs to align with _writeMaterialBundleHeader in pipeline/n64_libdragon/MaterialBundleWriter.js
typedef struct {
    uint32_t image_count;
    uint32_t texture_count;
    uint32_t material_count;
} fw64MaterialBundleInfo;

struct fw64MaterialBundle {
    fw64MaterialBundleInfo info;
    fw64Image* images;
    fw64Texture* textures;
    fw64Material* materials;
};