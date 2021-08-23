#pragma once

#include "framework64/renderer.h"
#include "framework64/desktop/sprite_renderer.h"

#include <SDL2/SDL.h>

#include <gl/glew.h>

#include <array>

#include <string>
#include <vector>

struct fw64Renderer {
    SDL_Window* window;
    SDL_GLContext gl_context;
    int screen_width;
    int screen_height;
    std::array<float, 4> clear_color = {0.10f, 0.0f, 0.65f, 1.0f};

    fw64RenderMode render_mode;
    framework64::SpriteRenderer sprite_renderer;
};

bool fw64_desktop_renderer_init(fw64Renderer* renderer, int screen_width, int screen_height, std::string const& base_path);

uint32_t fw64_desktop_load_shader(std::string const& vertex_path, std::string const& frag_path);
uint32_t fw64_desktop_renderer_compile_shader(std::string const& vertex, std::string const& fragment);