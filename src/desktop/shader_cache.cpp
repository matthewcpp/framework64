#include "framework64/desktop/shader_cache.h"

#include <limits>

namespace framework64 {

void ShaderCache::setShaderProgram(fw64Primitive & primitive) {
    Shader* shader = getShader(primitive);

    if (!shader) {
        primitive.material.shader = nullptr;
        return;
    }

    // check for existing shader
    auto program_hash = programHash(primitive);
    setProgram(primitive, shader, program_hash);
}

ShaderProgram* ShaderCache::particleShaderProgram() {
    auto result = shader_programs.find(&particle_shader);

    if (result != shader_programs.end())
        return result->second.get();

    auto program = particle_shader.create(std::numeric_limits<uint32_t>::max(), std::numeric_limits<uint32_t>::max(), shader_dir);
    program->hash = std::numeric_limits<uint64_t>::max();
    program->shader = &particle_shader;
    shader_programs.insert(std::make_pair(&particle_shader, program));

    return program;
}

uint64_t ShaderCache::programHash(fw64Primitive const & primitive) const {
    return ( static_cast<uint64_t>(primitive.attributes) << 32) | static_cast<uint64_t>(primitive.material.featureMask());
}

// TODO: This will continue to get more involved
Shader* ShaderCache::getShader(fw64Primitive const & primitive) {
    if (primitive.mode == fw64Primitive::Mode::Triangles) {
        if ((primitive.attributes & fw64Primitive::Attributes::VertexColors) ||
            (primitive.attributes & fw64Primitive::Attributes::Normals) == 0)
            return &vertex_color_shader;
        else
            return &gouraud_shader;

    }
    else if (primitive.mode == fw64Primitive::Mode::Lines)
        return &line_shader;

    return nullptr;
}

void ShaderCache::setProgram(fw64Primitive & primitive, Shader* shader, uint64_t program_hash) {
    auto result = shader_programs.equal_range(shader);

    for (auto it = result.first; it != result.second; ++it) {
        if (it->second->hash == program_hash) {
            primitive.material.shader = it->second.get();
            return;
        }
    }

    auto * program = shader->create(primitive.attributes, primitive.material.featureMask(), shader_dir);
    program->hash = program_hash;
    program->shader = shader;
    shader_programs.insert(std::make_pair(shader, program));

    primitive.material.shader = program;
}

}
