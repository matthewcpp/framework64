#include "framework64/desktop/material.h"
#include "framework64/desktop/texture.h"

// C Interface

void fw64_material_set_texture(fw64Material* material, fw64Texture* texture, int initial_frame) {
    material->texture = texture;
    fw64_material_set_texture_frame(material, initial_frame);
}

fw64Texture* fw64_material_get_texture(fw64Material* material) {
    return material->texture;
}

void fw64_material_set_texture_frame(fw64Material* material, int frame) {
    material->texture_frame = frame;
}

int fw64_material_get_texture_frame(fw64Material* material) {
    return material->texture_frame;
}

void fw64_material_set_shading_mode(fw64Material* material, fw64ShadingMode mode) {
    return; //n64-required api change. stubbed on desktop for now.
}