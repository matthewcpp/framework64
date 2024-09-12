#include "framework64/n64_libultra/material.h"

#include "framework64/n64_libultra/texture.h"
#include "framework64/n64_libultra/image.h"

#include <limits.h>

void fw64_n64_material_init(fw64Material* material) {
    fw64_color_rgba8_set(&material->color, 255, 255, 255, 255);
    material->texture = NULL;
    material->texture_frame = 0;
    material->shading_mode = FW64_SHADING_MODE_UNSET;
}

fw64Texture* fw64_material_get_texture(fw64Material* material) {
    return material->texture;
}

void fw64_material_set_texture(fw64Material* material, fw64Texture* texture, uint32_t initial_frame) {
    material->texture = texture;
    material->texture_frame = UINT_MAX;
    fw64_material_set_texture_frame(material, initial_frame);
}

void fw64_material_set_texture_frame(fw64Material* material, uint32_t frame) {
    if (material->texture && frame != material->texture_frame) {
        fw64_image_load_frame(material->texture->image, frame);
        material->texture_frame = frame;
    }
}

uint32_t fw64_material_get_texture_frame(fw64Material* material) {
    return material->texture_frame;
}

void fw64_material_set_shading_mode(fw64Material* material, fw64ShadingMode mode) {
    material->shading_mode = mode;
}

fw64ShadingMode fw64_material_get_shading_mode(const fw64Material* material) {
    return material->shading_mode;
}

void fw64_material_set_color(fw64Material* material, fw64ColorRGBA8 color) {
    material->color = color;
}

fw64ColorRGBA8 fw64_material_get_color(fw64Material* material) {
    return material->color;
}