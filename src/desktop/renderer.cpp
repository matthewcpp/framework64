#include "framework64/renderer.h"
#include "framework64/desktop/renderer.h"

#include <gl/glew.h>

int fw64_desktop_renderer_init(fw64Renderer* renderer, int screen_width, int screen_height) {
    renderer->window = SDL_CreateWindow("framework64", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_width, screen_height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    renderer->gl_context = SDL_GL_CreateContext(renderer->window );

    GLenum err = glewInit();

    if (err != GLEW_OK) {
        return 1;
    }

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