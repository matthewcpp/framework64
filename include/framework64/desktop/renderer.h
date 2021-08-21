#pragma once

#include "framework64/renderer.h"

#include <SDL2/SDL.h>

#include <array>

struct fw64Renderer {
    SDL_Window* window;
    SDL_GLContext gl_context;
    std::array<float, 4> clear_color = {0.0f, 0.0f, 0.0f, 1.0f};
};

int fw64_desktop_renderer_init(fw64Renderer* renderer, int screen_width, int screen_height);