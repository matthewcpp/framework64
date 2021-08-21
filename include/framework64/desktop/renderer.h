#pragma once

#include "framework64/renderer.h"

#include <SDL2/SDL.h>

#include <gl/glew.h>

#include <array>
#include <string>

struct SpriteShader{
    GLuint program;
    GLint uniform_matrix;
    GLint uniform_tex_sampler;
};

struct fw64Renderer {
    SDL_Window* window;
    SDL_GLContext gl_context;
    int screen_width;
    int screen_height;
    std::array<float, 4> clear_color = {0.0f, 0.0f, 0.0f, 1.0f};

    SpriteShader sprite_shader;
};

int fw64_desktop_renderer_init(fw64Renderer* renderer, int screen_width, int screen_height, std::string const& base_path);

bool fw64_desktop_init_sprite_shader(fw64Renderer* renderer, std::string const& shader_dir);
uint32_t fw64_desktop_load_shader(std::string const& vertex_path, std::string const& frag_path);
uint32_t fw64_desktop_renderer_compile_shader(std::string const& vertex, std::string const& fragment);