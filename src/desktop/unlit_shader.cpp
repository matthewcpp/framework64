#include "framework64/desktop/unlit_shader.hpp"

#include "framework64/desktop/mesh.hpp"

#include <memory>

namespace framework64 {

ShaderProgram* UnlitShader::create(uint32_t primitive_attributes, uint32_t material_features, std::string const & shader_dir) {
    std::string vertex_path = shader_dir + "unlit.vert.glsl";
    std::string frag_path = shader_dir + "unlit.frag.glsl";

    std::vector<std::string> preprocessor_statements;

    if (primitive_attributes & fw64Primitive::Attributes::VertexColors) {
        preprocessor_statements.emplace_back("#define FW64_VERTEX_COLORS");
    }

    bool has_diffuse_texture = material_features & fw64Material::Features::DiffuseTexture;
    if (has_diffuse_texture)
        preprocessor_statements.emplace_back("#define FW64_DIFFUSE_TEXTURE");

    GLuint handle = Shader::createFromPaths(vertex_path, frag_path, preprocessor_statements);

    if (!handle)
        return nullptr;

    auto program = std::make_unique<ShaderProgram>();
    program->shader = this;
    program->handle = handle;

    setupShaderProgram(program.get());

    if (program->mesh_transform_uniform_block_index == GL_INVALID_INDEX)
        return nullptr;

    return program.release();
}

void UnlitShader::setUniforms(ShaderProgram* program, fw64Material const & material) {
    glUniform4fv(material.shader->diffuse_color_location, 1, material.color.data());
    if (material.texture) {
        bindMaterialTexture(program, material);
    }
}

}

