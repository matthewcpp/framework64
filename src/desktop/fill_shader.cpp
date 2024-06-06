#include "framework64/desktop/fill_shader.hpp"

namespace framework64 {

void FillShader::setUniforms(ShaderProgram*, fw64Material const &) {

}

ShaderProgram* FillShader::create(uint32_t, uint32_t, std::string const & shader_dir) {
    std::string vertex_path = shader_dir + "fill.vert.glsl";
    std::string fragment_path = shader_dir + "fill.frag.glsl";

    std::vector<std::string> preprocessor_statements;

    auto program = std::make_unique<ShaderProgram>();
    program->shader = this;
    program->handle = Shader::createFromPaths(vertex_path, fragment_path, preprocessor_statements);

    if (!program->handle)
        return nullptr;

    program->mesh_transform_uniform_block_index = glGetUniformBlockIndex(program->handle, "fw64SpriteTransformData");

    if (program->mesh_transform_uniform_block_index == GL_INVALID_INDEX)
        return nullptr;

    return program.release();
}

}
