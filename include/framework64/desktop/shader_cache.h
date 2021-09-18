#pragma once

#include "framework64/desktop/gouraud_shader.h"
#include "framework64/desktop/line_shader.h"
#include "framework64/desktop/vertex_color_shader.h"
#include "framework64/desktop/mesh.h"

#include <memory>
#include <string>
#include <unordered_map>

namespace framework64 {

class ShaderCache {
public:
    ShaderCache(std::string shader_dir_path): shader_dir(shader_dir_path) {}

public:
    void setShaderProgram(fw64Primitive & primitive);

private:
    uint64_t programHash(fw64Primitive const & primitive) const;
    Shader* getShader(fw64Primitive const & primitive);

private:
    GouraudShader gouraud_shader;
    LineShader line_shader;
    VertexColorShader vertex_color_shader;

    std::unordered_multimap<Shader*, std::unique_ptr<ShaderProgram>> shader_programs;

    std::string shader_dir;
};

}