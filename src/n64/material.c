#include "framework64/n64/material.h"

#include "framework64/n64/texture.h"
#include "framework64/n64/image.h"

fw64Texture* fw64_material_get_texture(fw64Material* material) {
    return material->texture;
}

void fw64_material_set_texture(fw64Material* material, fw64Texture* texture, int initial_frame) {
    material->texture = texture;
    fw64_material_set_texture_frame(material, initial_frame);
}

void fw64_material_set_texture_frame(fw64Material* material, int frame) {
    material->texture_frame = frame;

    if (material->texture) {
        fw64_n64_image_load_frame(material->texture->image, frame);
    }
}

int fw64_material_get_texture_frame(fw64Material* material) {
    return material->texture_frame;
}

void fw64_material_set_shading_mode(fw64Material* material, fw64ShadingMode mode) {
    material->shading_mode = mode;
    return;
}
