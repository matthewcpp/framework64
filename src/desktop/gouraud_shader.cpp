#include "framework64/desktop/gouraud_shader.h"
#include "framework64/desktop/texture.h"

#include <memory>

namespace framework64 {

ShaderProgram* GouraudShader::create(uint32_t primitive_attributes, uint32_t material_features, std::string const & shader_dir) {
    std::string vertex_path = shader_dir + "gouraud.vert.glsl";
    std::string fragment_path = shader_dir + "gouraud.frag.glsl";

    std::vector<std::string> preprocessor_statements;

    bool has_diffuse_texture = material_features & fw64Material::Features::DiffuseTexture;
    if (has_diffuse_texture)
        preprocessor_statements.emplace_back("#define FW64_DIFFUSE_TEXTURE");

    auto program = std::make_unique<ShaderProgram>();
    program->shader = this;
    program->handle = Shader::createFromPaths(vertex_path, fragment_path, preprocessor_statements);

    if (!program->handle)
        return nullptr;

    if (has_diffuse_texture) {
        texture_info.uniform_block.create(3);
        // TODO: this should return a derived program object with this value, and the program should be passed to set uniforms
        texture_info_uniform_block_index = glGetUniformBlockIndex(program->handle, "fw64TextureFrameData");

        if (texture_info_uniform_block_index == GL_INVALID_INDEX)
            return nullptr;
    }

    program->lighting_data_uniform_block_index = glGetUniformBlockIndex(program->handle, "fw64LightingData");
    program->mesh_transform_uniform_block_index = glGetUniformBlockIndex(program->handle, "fw64MeshTransformData");
    program->diffuse_color_location = glGetUniformLocation(program->handle, "diffuse_color");
    program->diffuse_texture_location = glGetUniformLocation(program->handle, "diffuse_texture_sampler");

    if (    program->lighting_data_uniform_block_index == GL_INVALID_INDEX ||
            program->mesh_transform_uniform_block_index == GL_INVALID_INDEX ||
            program->diffuse_color_location == -1)
        return nullptr;

    if (has_diffuse_texture && program->diffuse_texture_location == -1)
        return nullptr;

    return program.release();
}

void GouraudShader::setUniforms(fw64Material const & material) {
    glUniform4fv(material.shader->diffuse_color_location, 1, material.color.data());

    if (material.texture) {
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

}