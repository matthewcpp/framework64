#pragma once

#include <framework64/material_bundle.h>

#include <framework64/n64/image.h>
#include <framework64/n64/material.h>
#include <framework64/n64/texture.h>

struct fw64MaterialBundle {
    uint32_t image_count;
    uint32_t texture_count;
    uint32_t material_count;
    uint32_t flags;
    fw64Image* images;
    fw64Texture* textures;
    fw64Material* materials;
};

void fw64_n64_material_bundle_init(fw64MaterialBundle* material_bundle, uint32_t image_count, uint32_t texture_count, uint32_t material_count, fw64Allocator* allocator);