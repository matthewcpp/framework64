#include "framework64/desktop/vertex_color_shader.h"

#include <memory>

namespace framework64 {

void VertexColorShader::setUniforms(fw64Material const & material) {}

ShaderProgram* VertexColorShader::create(uint32_t primitive_attributes, uint32_t material_features, std::string const & shader_dir) {
    std::string vertex_path = shader_dir + "unlit.vert.glsl";
    std::string frag_path = shader_dir + "unlit.frag.glsl";

    GLuint handle = Shader::createFromPaths(vertex_path, frag_path, {});

    if (!handle)
        return nullptr;

    auto program = std::make_unique<ShaderProgram>();
    program->shader = this;
    program->handle = handle;

    program->mesh_transform_uniform_block_index = glGetUniformBlockIndex(program->handle, "fw64MeshTransformData");

    if (program->mesh_transform_uniform_block_index == GL_INVALID_INDEX)
        return nullptr;

    return program.release();
}

}

