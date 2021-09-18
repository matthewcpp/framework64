#include "framework64/desktop/shader.h"

#include <gl/glew.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

namespace framework64 {
std::string const glsl_version_str = "#version 410";

GLuint Shader::createFromPaths(std::string const & vertex_path, std::string const & frag_path, std::vector<std::string> const & preprocessor_statements) {
    std::ifstream vertex_file(vertex_path);
    std::ifstream fragment_file(frag_path);

    if (!vertex_file || !fragment_file)
        return 0;

    auto read_file = [](std::string const& version_str, std::vector<std::string> const & preprocessor_statements, std::ifstream& file)->std::string{
        std::ostringstream str;
        str << version_str << '\n';

        for (auto const & preprocessor_statement : preprocessor_statements)
            str << preprocessor_statement << '\n';

        str << file.rdbuf();
        return str.str();
    };

    auto const vertex_shader = read_file(glsl_version_str, preprocessor_statements, vertex_file);
    auto const fragment_shader = read_file(glsl_version_str, preprocessor_statements, fragment_file);

    return compile(vertex_shader, fragment_shader);
}

static GLuint create_shader(std::string const & str, GLenum shader_type){
    const char* source = str.c_str();
    int source_length = static_cast<GLint>(str.length());
    GLuint shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, &source, &source_length);
    glCompileShader(shader);

    GLint shader_compiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &shader_compiled);

    if (!shader_compiled) {
        int info_log_size;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_log_size);
        std::vector<char> compile_error_message;
        compile_error_message.resize(info_log_size);

        glGetShaderInfoLog(shader, info_log_size, nullptr, compile_error_message.data());
        std::string shader_type_str = (shader_type == GL_VERTEX_SHADER) ? "vertex" : "fragment";
        std::cout << "Error compiling " <<  shader_type_str << ": " << compile_error_message.data() << std::endl;
        std::cout << str << std::endl;

        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

GLuint Shader::compile(std::string const& vertex, std::string const& fragment) {
    GLuint vertex_shader = create_shader(vertex, GL_VERTEX_SHADER);
    if (!vertex_shader) {
        return 0;
    }

    GLuint fragment_shader = create_shader(fragment, GL_FRAGMENT_SHADER);

    if (!fragment_shader) {
        glDeleteShader(vertex_shader);
        return 0;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);

    glLinkProgram(program);

    GLint link_status;
    glGetProgramiv(program, GL_LINK_STATUS, &link_status);

    if (!link_status) {
        std::cout << "failed to link shader.";
        return 0;
    }

    return program;
}

}