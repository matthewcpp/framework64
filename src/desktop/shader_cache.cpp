#include "framework64/desktop/shader_cache.hpp"
#include "framework64/desktop/mesh.hpp"

#include <limits>

namespace framework64 {

static uint64_t programHash(uint32_t primitive_attributes, uint32_t material_features) {
    return ( static_cast<uint64_t>(primitive_attributes) << 32) | static_cast<uint64_t>(material_features);
}

ShaderProgram* ShaderCache::getShaderProgram(fw64ShadingMode shading_mode) {
    uint32_t primitive_attributes = fw64Primitive::Attributes::Positions;
    uint32_t material_features = fw64Material::Features::None;
    Shader* shader = nullptr;

    switch(shading_mode) {
        case FW64_SHADING_MODE_VERTEX_COLOR: {
            primitive_attributes = fw64Primitive::Attributes::Positions | fw64Primitive::Attributes::VertexColors;
            shader = &unlit_shader;
            break;
        }

        case FW64_SHADING_MODE_VERTEX_COLOR_TEXTURED: {
            primitive_attributes = fw64Primitive::Attributes::Positions | fw64Primitive::Attributes::VertexColors | fw64Primitive::Attributes::TexCoords;
            material_features = fw64Material::Features::DiffuseTexture;
            shader = &unlit_shader;
            break;
        }

        case FW64_SHADING_MODE_GOURAUD: {
            primitive_attributes = fw64Primitive::Attributes::Positions | fw64Primitive::Attributes::Normals;
            shader = &gouraud_shader;
            break;
        }

        case FW64_SHADING_MODE_GOURAUD_TEXTURED: {
            primitive_attributes = fw64Primitive::Attributes::Positions | fw64Primitive::Attributes::Normals | fw64Primitive::Attributes::TexCoords;
            material_features = fw64Material::Features::DiffuseTexture;
            shader = &gouraud_shader;
            break;
        }

        case FW64_SHADING_MODE_UNLIT_TEXTURED: {
            primitive_attributes = fw64Primitive::Attributes::Positions| fw64Primitive::Attributes::TexCoords;
            material_features = fw64Material::Features::DiffuseTexture;
            shader = &unlit_shader;
            break;
        }

        case FW64_SHADING_MODE_SPRITE: {
            primitive_attributes = fw64Primitive::Attributes::Positions | fw64Primitive::Attributes::TexCoords;
            material_features = fw64Material::Features::DiffuseTexture;
            shader = &sprite_shader;
            break;
        }

        case FW64_SHADING_MODE_LINE: {
            primitive_attributes = fw64Primitive::Attributes::Positions;
            shader = &line_shader;
            break;
        }

        case FW64_SHADING_MODE_FILL: {
            primitive_attributes = fw64Primitive::Attributes::Positions | fw64Primitive::Attributes::VertexColors;
            shader = &fill_shader;
            break;
        }

        case FW64_SHADING_MODE_DECAL_TEXTURE:
            // TODO: This needs to be implemented
        case FW64_SHADING_MODE_UNSET:
        case FW64_SHADING_MODE_COUNT:
            break;
    }

    uint64_t program_hash = programHash(primitive_attributes, material_features);

    auto result = shader_programs.equal_range(shader);

    for (auto it = result.first; it != result.second; ++it) {
        if (it->second->hash == program_hash) {
            return it->second.get();
        }
    }

    auto * program = shader->create(primitive_attributes, material_features, shader_dir);
    program->hash = program_hash;
    program->shader = shader;
    shader_programs.insert(std::make_pair(shader, program));

    return program;
}

}
