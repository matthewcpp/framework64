#include "framework64/desktop/material.hpp"
#include "framework64/desktop/shader_cache.hpp"
#include "framework64/desktop/texture.hpp"

void fw64Material::updateShader() {

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


void fw64_material_set_color(fw64Material* material, uint8_t r, uint8_t g, uint8_t b) {
    material->color[0] = static_cast<float>(r) / 255.0f;
    material->color[1] = static_cast<float>(g) / 255.0f;
    material->color[2] = static_cast<float>(b) / 255.0f;
}

void fw64_material_get_color(fw64Material* material, fw64ColorRGBA8* color) {
    color->r = static_cast<uint8_t>(material->color[0] * 255.0f);
    color->g = static_cast<uint8_t>(material->color[1] * 255.0f);
    color->b = static_cast<uint8_t>(material->color[2] * 255.0f);
    color->a = static_cast<uint8_t>(material->color[3] * 255.0f);
}
