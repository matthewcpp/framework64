#pragma once

#include "framework64/desktop/shader.h"
#include "framework64/desktop/uniform_block.h"

namespace framework64 {

class ParticleShader : public Shader {
public:
    void setUniforms(fw64Material const & material) override;
    ShaderProgram* create(uint32_t primitive_attributes, uint32_t material_features, std::string const & shader_dir) override;

private:
    struct ParticleShaderUniformData{
        float slice_top;
        float slice_left;
        float slice_width;
        float slice_height;
    };

    GLuint texture_info_uniform_block_index = GL_INVALID_INDEX;

    UniformBlock<ParticleShaderUniformData> texture_info_uniform_block;
};

}