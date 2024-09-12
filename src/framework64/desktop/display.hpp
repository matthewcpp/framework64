#pragma once

#include "framework64/desktop/settings.hpp"

#include <SDL2/SDL.h>

struct fw64Display {
public:
    bool init(const framework64::Settings& settings);
    bool initWithoutWindow(int width, int height);
    void swap();

public:
    inline int framebuffer_width() const { return _framebuffer_width; }
    inline int framebuffer_height() const { return _framebuffer_height; }
    inline int window_width() const { return _window_width; }
    inline int window_height() const { return _window_height; }

private:
    int _framebuffer_width, _framebuffer_height;
    int _window_width, _window_height;

private:
    SDL_Window* window;
    SDL_GLContext gl_context;
};

struct fw64Displays {
    fw64Displays(fw64Display* primary) : primary(primary) {};

    fw64Display* primary;
};
