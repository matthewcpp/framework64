#include "framework64/desktop/shader_cache.h"

namespace framework64 {

ShaderProgram* ShaderCache::getShaderProgram(fw64Mesh::Primitive const & primitive) {
    Shader* shader = getShader(primitive);

    if (!shader)
        return nullptr;

    // check for existing shader
    auto program_hash = programHash(primitive);
    auto result = shader_programs.equal_range(shader);

    for (auto it = result.first; it != result.second; ++it) {
        if (it->second->hash == program_hash)
            return it->second.get();
    }

    auto * program = shader->create(primitive.attributes, primitive.material.featureMask(), shader_dir);
    shader_programs.insert(std::make_pair(shader, program));

    return program;
}

uint64_t ShaderCache::programHash(fw64Mesh::Primitive const & primitive) const {
    return ( static_cast<uint64_t>(primitive.attributes) << 32) | static_cast<uint64_t>(primitive.material.featureMask());
}

// TODO: This will get more involved
Shader* ShaderCache::getShader(fw64Mesh::Primitive const & primitive) {
    if (primitive.mode == fw64Mesh::Primitive::Mode::Triangles)
        return &gouraud_shader;

    return nullptr;
}

}
