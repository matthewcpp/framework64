#include "framework64/desktop/display.hpp"

#include "framework64/display.h"

#ifdef __linux__
#include <GL/glew.h>
#else
#include <gl/glew.h>
#endif

#include <iostream>

bool fw64Display::init(const framework64::Settings& settings) {
    _framebuffer_width = settings.screen_width;
    _framebuffer_height = settings.screen_height;

    _window_width = static_cast<int>(settings.screen_width * settings.display_scale);
    _window_height = static_cast<int>(settings.screen_height * settings.display_scale);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    window = SDL_CreateWindow(settings.application_name.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, _window_width, _window_height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    
    if (window == nullptr) {
        std::cout << "Could not create SDL window: " << SDL_GetError() << std::endl;
        return false;
    }

    gl_context = SDL_GL_CreateContext(window);

    GLenum err = glewInit();

    if (err != GLEW_OK) {
        return false;
    }

    return true;
}

bool fw64Display::initWithoutWindow(int width, int height) {
    _framebuffer_width = width;
    _framebuffer_height = height;

    _window_width = width;
    _window_height = height;

    return true;
}

void fw64Display::swap() {
    SDL_GL_SwapWindow(window);
}

// C-API

fw64Display* fw64_displays_get_primary(fw64Displays* displays) {
    return displays->primary;
}

IVec2 fw64_display_get_size(const fw64Display* display) {
    return {display->framebuffer_width(), display->framebuffer_height()};
}

Vec2 fw64_display_get_size_f(const fw64Display* display) {
    return {static_cast<float>(display->framebuffer_width()), static_cast<float>(display->framebuffer_height())};
}