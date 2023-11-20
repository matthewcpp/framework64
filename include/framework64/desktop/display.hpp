#pragma once

#include "framework64/desktop/settings.hpp"

#include <SDL2/SDL.h>

namespace framework64 {

class Display {
public:
    bool init(const Settings& settings);
    void swap();

public:
    inline int width() const { return window_width; }
    inline int height() const { return window_height; }

private:
    int window_width, window_height;

private:
    SDL_Window* window;
    SDL_GLContext gl_context;
};

}