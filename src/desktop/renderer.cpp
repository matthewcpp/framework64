#include "framework64/renderer.h"
#include "framework64/desktop/renderer.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

int fw64_desktop_renderer_init(fw64Renderer* renderer, int screen_width, int screen_height, std::string const& base_path) {
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );

    renderer->window = SDL_CreateWindow("framework64", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_width, screen_height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    renderer->gl_context = SDL_GL_CreateContext(renderer->window );

    GLenum err = glewInit();

    if (err != GLEW_OK) {
        return 1;
    }

    std::cout << "Created OpenGL context: " << glGetString(GL_VERSION) << std::endl;

    std::string shader_dir = base_path + "glsl/";
    fw64_desktop_init_sprite_shader(renderer, shader_dir);

    return 0;
}

void fw64_renderer_set_clear_color(fw64Renderer* renderer, Color* color) {
    renderer->clear_color[0] = color->r / 255.0f;
    renderer->clear_color[1] = color->g / 255.0f;
    renderer->clear_color[2] = color->b /255.0f;

    glClearColor(renderer->clear_color[0], renderer->clear_color[1], renderer->clear_color[2], 1.0f);
}

void fw64_renderer_begin(fw64Renderer* renderer, fw64Camera* camera, fw64RenderMode render_mode, fw64RendererFlags flags) {
    if ((flags & FW64_RENDERER_FLAG_CLEAR) == FW64_RENDERER_FLAG_CLEAR) {
        glViewport(0,0, renderer->screen_width, renderer->screen_height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
}

void fw64_renderer_end(fw64Renderer* renderer, fw64RendererFlags flags) {
    if ((flags & FW64_RENDERER_FLAG_SWAP) == FW64_RENDERER_FLAG_SWAP){
        SDL_GL_SwapWindow(renderer->window);
    }
}

void fw64_renderer_draw_static_mesh(fw64Renderer* renderer, fw64Transform* transform, fw64Mesh* mesh) {

}

void fw64_renderer_set_fill_color(fw64Renderer* renderer, Color* color) {

}

void fw64_renderer_set_fill_mode(fw64Renderer* renderer) {

}

void fw64_renderer_draw_filled_rect(fw64Renderer* renderer, IRect* rect) {

}

void fw64_renderer_draw_sprite(fw64Renderer* renderer, fw64Texture* sprite, int x, int y) {

}

void fw64_renderer_draw_sprite_slice(fw64Renderer* renderer, fw64Texture* sprite, int frame, int x, int y) {

}

void fw64_renderer_draw_text(fw64Renderer* renderer, fw64Font* font, int x, int y, const char* text) {

}

void fw64_renderer_get_screen_size(fw64Renderer* renderer, IVec2* screen_size) {

}

bool fw64_desktop_init_sprite_shader(fw64Renderer* renderer, std::string const& shader_dir) {
    const std::string vertex_path = shader_dir + "sprite.vert.glsl";
    const std::string frag_path = shader_dir + "sprite.frag.glsl";

    renderer->sprite_shader.program = fw64_desktop_load_shader(vertex_path, frag_path);
    if (!renderer->sprite_shader.program) {
        return false;
    }

    renderer->sprite_shader.uniform_matrix = glGetUniformLocation(renderer->sprite_shader.program, "matrix");
    renderer->sprite_shader.uniform_tex_sampler = glGetUniformLocation(renderer->sprite_shader.program, "tex_sampler");
}

uint32_t fw64_desktop_load_shader(std::string const& vertex_path, std::string const& frag_path) {
    std::ifstream vertex_file(vertex_path);
    std::ifstream fragment_file(frag_path);

    if (!vertex_file || !fragment_file)
        return 0;

    auto read_file = [](std::ifstream& file)->std::string{
        std::ostringstream str;
        str << file.rdbuf();
        return str.str();
    };

    auto const vertex_shader = read_file(vertex_file);
    auto const fragment_shader = read_file(fragment_file);

    return fw64_desktop_renderer_compile_shader(vertex_shader, fragment_shader);
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

uint32_t fw64_desktop_renderer_compile_shader(std::string const& vertex, std::string const& fragment){
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