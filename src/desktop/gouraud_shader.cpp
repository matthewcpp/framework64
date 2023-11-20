#include "framework64/desktop/gouraud_shader.hpp"
#include "framework64/desktop/texture.hpp"

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

    setupShaderProgram(program.get());

    if (!validateProgram(program.get(), primitive_attributes, material_features))
        return nullptr;

    return program.release();
}

bool GouraudShader::validateProgram(ShaderProgram* program, uint32_t primitive_attributes, uint32_t material_features) {
    bool has_diffuse_texture = material_features & fw64Material::Features::DiffuseTexture;

    if (    program->lighting_data_uniform_block_index == GL_INVALID_INDEX ||
            program->mesh_transform_uniform_block_index == GL_INVALID_INDEX ||
            program->diffuse_color_location == -1)
        return false;

    if (has_diffuse_texture && program->diffuse_texture_location == -1)
        return false;

    if (has_diffuse_texture && program->texture_info_uniform_block_index == GL_INVALID_INDEX)
        return false;

    return true;
}


void GouraudShader::setUniforms(ShaderProgram* program, fw64Material const & material) {
    glUniform4fv(material.shader->diffuse_color_location, 1, material.color.data());

    if (material.texture) {
        bindMaterialTexture(program, material);
    }
}

}