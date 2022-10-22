#pragma once

#include "framework64/desktop/fill_shader.h"
#include "framework64/desktop/gouraud_shader.h"
#include "framework64/desktop/line_shader.h"
#include "framework64/desktop/sprite_shader.h"
#include "framework64/desktop/unlit_shader.h"

#include <memory>
#include <string>
#include <unordered_map>

namespace framework64 {

class ShaderCache {
public:
    ShaderCache(std::string shader_dir_path): shader_dir(shader_dir_path) {}

public:
    ShaderProgram* getShaderProgram(fw64ShadingMode shading_mode);

    inline std::string shaderDir() const { return shader_dir; }

private:
    GouraudShader gouraud_shader;
    LineShader line_shader;
    SpriteShader sprite_shader;
    UnlitShader unlit_shader;
    FillShader fill_shader;

    std::unordered_multimap<Shader*, std::unique_ptr<ShaderProgram>> shader_programs;

    std::string shader_dir;
};

}
