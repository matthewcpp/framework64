#pragma once

#include "framework64/renderer.h"
#include "framework64/desktop/mesh_renderer.h"
#include "framework64/desktop/shader_cache.h"
#include "framework64/desktop/sprite_renderer.h"


#include <SDL2/SDL.h>

#ifdef __linux__
#include <GL/glew.h>
#else
#include <gl/glew.h>
#endif

#include <array>

#include <string>
#include <vector>

struct fw64Renderer {
public:
    bool init(int screen_width, int screen_height, std::string const & shader_dir_path);

    void setClearColor(float r, float g, float b, float a);
    void begin(fw64Camera* cam, fw64RenderMode new_render_mode, fw64RendererFlags flags);
    void end(fw64RendererFlags flags);
    void setScreenSize(int width, int height);

public:
    framework64::SpriteRenderer sprite_renderer;
    framework64::MeshRenderer mesh_renderer;
    fw64RenderMode render_mode;
    int screen_width;
    int screen_height;
    std::array<float, 4> clear_color = {0.0f, 0.0f, 0.0f, 1.0f};
    fw64Camera* camera = nullptr;
private:
    SDL_Window* window;
    SDL_GLContext gl_context;
};