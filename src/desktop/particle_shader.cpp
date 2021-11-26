#include "framework64/desktop/particle_shader.h"
#include "framework64/desktop/texture.h"

#include <memory>

namespace framework64 {

ShaderProgram* ParticleShader::create(uint32_t primitive_attributes, uint32_t material_features, std::string const & shader_dir) {
    std::string vertex_path = shader_dir + "particle.vert.glsl";
    std::string fragment_path = shader_dir + "particle.frag.glsl";

    std::vector<std::string> preprocessor_statements;

    auto program = std::make_unique<ShaderProgram>();
    program->shader = this;
    program->handle = Shader::createFromPaths(vertex_path, fragment_path, preprocessor_statements);

    if (!program->handle)
        return nullptr;

    texture_info.uniform_block.create(4);
    program->mesh_transform_uniform_block_index = glGetUniformBlockIndex(program->handle, "fw64MeshTransformData");
    program->diffuse_texture_location = glGetUniformLocation(program->handle, "diffuse_texture_sampler");

    // TODO: this should return a derived program object with this value, and the program should be passed to set uniforms
    texture_info_uniform_block_index = glGetUniformBlockIndex(program->handle, "fw64TextureFrameData");

    if (program->mesh_transform_uniform_block_index == GL_INVALID_INDEX || program->diffuse_texture_location == -1)
        return nullptr;

    if (texture_info_uniform_block_index == GL_INVALID_INDEX)
        return nullptr;

    return program.release();
}

void ParticleShader::setUniforms(fw64Material const & material) {
    texture_info.setUniformData(material);
    texture_info.uniform_block.update();
    glUniformBlockBinding(material.shader->handle, texture_info_uniform_block_index, texture_info.uniform_block.binding_index);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, material.texture->image->gl_handle);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, material.texture->wrap_s);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, material.texture->wrap_t);
    glUniform1i(material.shader->diffuse_texture_location, 0);
}

}