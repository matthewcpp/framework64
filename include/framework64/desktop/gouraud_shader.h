#pragma once

#include "framework64/desktop/shader.h"
#include "framework64/desktop/texture_info_uniform.h"

namespace framework64 {

class GouraudShader : public Shader {
public:
    virtual ShaderProgram* create(uint32_t primitive_attributes, uint32_t material_features, std::string const & shader_dir) override;
    virtual void setUniforms(fw64Material const & material) override;

    TextureInfoUniform texture_info;

    GLuint texture_info_uniform_block_index = GL_INVALID_INDEX;
};

}