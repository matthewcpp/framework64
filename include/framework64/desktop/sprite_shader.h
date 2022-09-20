#pragma once

#include "framework64/desktop/shader.h"
#include "framework64/desktop/texture_info_uniform.h"
#include "framework64/desktop/uniform_block.h"

namespace framework64 {

class SpriteShader : public Shader {
public:
    void setUniforms(ShaderProgram* program, fw64Material const & material) override;
    ShaderProgram* create(uint32_t primitive_attributes, uint32_t material_features, std::string const & shader_dir) override;
};

}
