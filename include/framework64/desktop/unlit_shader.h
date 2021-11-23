#pragma once

#include "framework64/desktop/shader.h"

namespace framework64 {

class UnlitShader : public Shader {
public:
    virtual void setUniforms(fw64Material const & material) override;
    virtual ShaderProgram* create(uint32_t primitive_attributes, uint32_t material_features, std::string const & shader_dir) override;
};

}
