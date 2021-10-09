#include "framework64/desktop/material.h"
#include "framework64/desktop/texture.h"

// C Interface

fw64Texture* fw64_material_get_texture(fw64Material* material) {
    return material->texture;
}

void fw64_material_set_texture_frame(fw64Material* material, int frame) {
    material->texture_frame = frame;
}

int fw64_material_get_texture_frame(fw64Material* material) {
    return material->texture_frame;
}