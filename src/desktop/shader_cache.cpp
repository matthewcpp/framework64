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

void ShaderCache::setParticleShader(fw64Primitive & primitive) {
    setProgram(primitive, &particle_shader, std::numeric_limits<uint64_t>::max());
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
        if (it->second->hash == program_hash)
            primitive.material.shader = it->second.get();
    }

    auto * program = shader->create(primitive.attributes, primitive.material.featureMask(), shader_dir);
    program->hash = program_hash;
    shader_programs.insert(std::make_pair(shader, program));

    primitive.material.shader = program;
}

}
