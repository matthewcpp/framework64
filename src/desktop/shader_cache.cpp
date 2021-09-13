#include "framework64/desktop/shader_cache.h"

namespace framework64 {

void ShaderCache::setShaderProgram(fw64Mesh::Primitive & primitive) {
    Shader* shader = getShader(primitive);

    if (!shader) {
        primitive.material.shader = nullptr;
        return;
    }

    // check for existing shader
    auto program_hash = programHash(primitive);
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

uint64_t ShaderCache::programHash(fw64Mesh::Primitive const & primitive) const {
    return ( static_cast<uint64_t>(primitive.attributes) << 32) | static_cast<uint64_t>(primitive.material.featureMask());
}

// TODO: This will get more involved
Shader* ShaderCache::getShader(fw64Mesh::Primitive const & primitive) {
    if (primitive.mode == fw64Mesh::Primitive::Mode::Triangles) {
        if (primitive.attributes & fw64Mesh::Primitive::Attributes::VertexColors)
            return &vertex_color_shader;
        else
            return &gouraud_shader;

    }
    else if (primitive.mode == fw64Mesh::Primitive::Mode::Lines)
        return &line_shader;

    return nullptr;
}

}
