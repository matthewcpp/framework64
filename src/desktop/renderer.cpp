#include "framework64/renderer.h"
#include "framework64/desktop/renderer.h"

#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

bool fw64_desktop_renderer_init(fw64Renderer* renderer, int screen_width, int screen_height, std::string const& base_path) {
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    renderer->window = SDL_CreateWindow("framework64", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_width, screen_height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    renderer->gl_context = SDL_GL_CreateContext(renderer->window );

    renderer->screen_width = screen_width;
    renderer->screen_height = screen_height;

    GLenum err = glewInit();

    if (err != GLEW_OK) {
        return false;
    }

    std::cout << "Created OpenGL context: " << glGetString(GL_VERSION) << std::endl;

    glClearColor(renderer->clear_color[0], renderer->clear_color[1], renderer->clear_color[2], renderer->clear_color[3]);

    std::string shader_dir = base_path + "glsl/";

    if (!renderer->sprite_renderer.init(shader_dir)) return false;
    renderer->sprite_renderer.setScreenSize(screen_width, screen_height);

    return true;
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

    renderer->render_mode = render_mode;

    switch(renderer->render_mode) {
        case FW64_RENDERER_MODE_SPRITES:
            renderer->sprite_renderer.begin(camera);
            break;
    }
}

void fw64_renderer_end(fw64Renderer* renderer, fw64RendererFlags flags) {
    switch(renderer->render_mode) {
        case FW64_RENDERER_MODE_SPRITES:
            renderer->sprite_renderer.end();
            break;
    }

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

void fw64_renderer_draw_sprite(fw64Renderer* renderer, fw64Texture* texture, int x, int y) {
    assert(renderer->render_mode == FW64_RENDERER_MODE_SPRITES);
    renderer->sprite_renderer.drawSprite(texture, x, y);
}

void fw64_renderer_draw_sprite_slice(fw64Renderer* renderer, fw64Texture* texture, int frame, int x, int y) {
    assert(renderer->render_mode == FW64_RENDERER_MODE_SPRITES);
    renderer->sprite_renderer.drawSpriteFrame(texture, frame, x, y);
}

void fw64_renderer_draw_text(fw64Renderer* renderer, fw64Font* font, int x, int y, const char* text) {
}

void fw64_renderer_get_screen_size(fw64Renderer* renderer, IVec2* screen_size) {
    screen_size->x = renderer->screen_width;
    screen_size->y = renderer->screen_height;
}
