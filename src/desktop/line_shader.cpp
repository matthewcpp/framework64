#include "framework64/desktop/line_shader.h"

#include <memory>

namespace framework64 {

ShaderProgram* LineShader::create(uint32_t primitive_attributes, uint32_t material_features, std::string const & shader_dir) {
    std::string const vertex_path = shader_dir + "line.vert.glsl";
    std::string const frag_path = shader_dir + "line.frag.glsl";

    GLuint handle = Shader::createFromPaths(vertex_path, frag_path, {});

    if (!handle)
        return nullptr;

    auto program = std::make_unique<ShaderProgram>();
    program->shader = this;
    program->handle = handle;

    program->mesh_transform_uniform_block_index = glGetUniformBlockIndex(program->handle, "fw64MeshTransformData");
    program->diffuse_color_location = glGetUniformLocation(program->handle, "diffuse_color");

    if (program->mesh_transform_uniform_block_index == GL_INVALID_INDEX || program->diffuse_color_location)
        return nullptr;

    return program.release();
}

void LineShader::setUniforms(Material const & material) {
    glUniform4fv(material.shader->diffuse_color_location, 1, material.color.data());
}

}
