#include "framework64/desktop/material.hpp"
#include "framework64/desktop/shader_cache.hpp"
#include "framework64/desktop/texture.hpp"

void fw64Material::updateShader() {
    shader = shader_cache.getShaderProgram(shading_mode);
}

// C Interface

void fw64_material_set_texture(fw64Material* material, fw64Texture* texture, uint32_t initial_frame) {
    material->texture = texture;
    fw64_material_set_texture_frame(material, initial_frame);
}

fw64Texture* fw64_material_get_texture(fw64Material* material) {
    return material->texture;
}

void fw64_material_set_texture_frame(fw64Material* material, uint32_t frame) {
    material->texture_frame = frame;
}

uint32_t fw64_material_get_texture_frame(fw64Material* material) {
    return material->texture_frame;
}

void fw64_material_set_shading_mode(fw64Material* material, fw64ShadingMode mode) {
    material->shading_mode = mode;
    material->updateShader();
}

fw64ShadingMode fw64_material_get_shading_mode(const fw64Material* material) {
    return material->shading_mode;
}


void fw64_material_set_color(fw64Material* material, fw64ColorRGBA8 color) {
    material->color[0] = static_cast<float>(color.r) / 255.0f;
    material->color[1] = static_cast<float>(color.g) / 255.0f;
    material->color[2] = static_cast<float>(color.b) / 255.0f;
    material->color[3] = static_cast<float>(color.a) / 255.0f;
}

fw64ColorRGBA8 fw64_material_get_color(fw64Material* material) {
    return {
       static_cast<uint8_t>(material->color[0] * 255.0f),
       static_cast<uint8_t>(material->color[1] * 255.0f),
       static_cast<uint8_t>(material->color[2] * 255.0f),
       static_cast<uint8_t>(material->color[3] * 255.0f)
    };
}
