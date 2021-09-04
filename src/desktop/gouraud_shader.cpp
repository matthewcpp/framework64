#include "framework64/desktop/gouraud_shader.h"

#include <memory>

namespace framework64 {

ShaderProgram* GouraudShader::create(uint32_t primitive_attributes, uint32_t material_features, std::string const & shader_dir) {
    std::string vertex_path = shader_dir + "gouraud.vert.glsl";
    std::string fragment_path = shader_dir + "gouraud.frag.glsl";

    auto program = std::make_unique<ShaderProgram>();
    program->shader = this;
    program->handle = Shader::createFromPaths(vertex_path, fragment_path);

    if (!program->handle)
        return nullptr;


    program->lighting_data_uniform_block_index = glGetUniformBlockIndex(program->handle, "fw64LightingData");
    program->mesh_transform_uniform_block_index = glGetUniformBlockIndex(program->handle, "fw64MeshTransformData");
    program->diffuse_color_location = glGetUniformLocation(program->handle, "diffuse_color");

    if (    program->lighting_data_uniform_block_index == GL_INVALID_INDEX ||
            program->mesh_transform_uniform_block_index == GL_INVALID_INDEX ||
            program->diffuse_color_location == -1)
        return nullptr;

    return program.release();
}

void GouraudShader::setUniforms(Material const & material) {
    glUniform4fv(material.shader->diffuse_color_location, 1, material.color.data());
}

}