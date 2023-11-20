#include "framework64/desktop/sprite_shader.hpp"
#include "framework64/desktop/texture.hpp"

#include <memory>

namespace framework64 {

struct SpriteShaderProgram : public ShaderProgram {

};

ShaderProgram* SpriteShader::create(uint32_t primitive_attributes, uint32_t material_features, std::string const & shader_dir) {
    std::string vertex_path = shader_dir + "sprite.vert.glsl";
    std::string fragment_path = shader_dir + "sprite.frag.glsl";

    std::vector<std::string> preprocessor_statements;

    auto program = std::make_unique<ShaderProgram>();
    program->shader = this;
    program->handle = Shader::createFromPaths(vertex_path, fragment_path, preprocessor_statements);

    if (!program->handle)
        return nullptr;

    program->texture_info.uniform_block.create();
    program->mesh_transform_uniform_block_index = glGetUniformBlockIndex(program->handle, "fw64SpriteTransformData");
    program->diffuse_texture_location = glGetUniformLocation(program->handle, "diffuse_texture_sampler");

    // TODO: this should return a derived program object with this value, and the program should be passed to set uniforms
    program->texture_info_uniform_block_index = glGetUniformBlockIndex(program->handle, "fw64TextureFrameData");

    if (program->mesh_transform_uniform_block_index == GL_INVALID_INDEX || program->diffuse_texture_location == -1)
        return nullptr;

    if (program->texture_info_uniform_block_index == GL_INVALID_INDEX)
        return nullptr;

    return program.release();
}

void SpriteShader::setUniforms(ShaderProgram* program, fw64Material const & material) {
    bindMaterialTexture(program, material);
}

}