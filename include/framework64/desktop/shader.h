#pragma once

#include "framework64/desktop/material.h"
#include "framework64/desktop/texture_info_uniform.h"

#ifdef __linux__
#include <GL/glew.h>
#else
#include <gl/glew.h>
#endif

#include <string>
#include <vector>

namespace framework64 {

class ShaderProgram;

class Shader {
public:
    virtual void setUniforms(ShaderProgram* program, fw64Material const & material) = 0;
    virtual ShaderProgram* create(uint32_t primitive_attributes, uint32_t material_features, std::string const & shader_dir) = 0;

public:
    static GLuint createFromPaths(std::string const & vertex_path, std::string const & frag_path, std::vector<std::string> const & preprocessor_statements);
    static GLuint compile(std::string const& vertex, std::string const& fragment);

protected:
    static void setupShaderProgram(ShaderProgram* program);
    static void bindMaterialTexture(ShaderProgram* program, fw64Material const & material);
};

struct ShaderProgram {
    GLuint handle = 0;

    // the actual uniform blocks for these indices are managed by the renderer
    GLuint lighting_data_uniform_block_index = GL_INVALID_INDEX;
    GLuint mesh_transform_uniform_block_index = GL_INVALID_INDEX;
    GLuint fog_uniform_block_index = GL_INVALID_INDEX;

    TextureInfoUniform texture_info;
    GLuint texture_info_uniform_block_index = GL_INVALID_INDEX;

    GLint diffuse_color_location = -1;
    GLint diffuse_texture_location = -1;
    Shader* shader = nullptr;
    uint64_t hash = 0;
};

}