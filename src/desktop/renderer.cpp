#include "framework64/desktop/renderer.h"

#include <cassert>
#include <iostream>

bool fw64Renderer::init(int width, int height, framework64::ShaderCache& shader_cache) {
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    window = SDL_CreateWindow("framework64", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    gl_context = SDL_GL_CreateContext(window );

    GLenum err = glewInit();

    if (err != GLEW_OK) {
        return false;
    }

    std::cout << "Created OpenGL context: " << glGetString(GL_VERSION) << std::endl;

    glClearColor(clear_color[0], clear_color[1], clear_color[2], clear_color[3]);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (!sprite_renderer.init(shader_cache)) return false;
    if (!mesh_renderer.init(shader_cache)) return false;
    framebuffer_write_texture.texture.init();
    setScreenSize(width, height);

    framebuffer_write_texture.texture.material.shader = shader_cache.particleShaderProgram();

    return true;
}

void fw64Renderer::setClearColor(float r, float g, float b, float a) {
    clear_color = {r, g, b, a};
    glClearColor(clear_color[0], clear_color[1], clear_color[2], 1.0f);
}

void fw64Renderer::begin(fw64Camera* cam, fw64RenderMode new_render_mode, fw64RendererFlags flags) {
    camera = cam;

    if ((flags & FW64_RENDERER_FLAG_CLEAR) == FW64_RENDERER_FLAG_CLEAR) {
        glViewport(0,0, screen_width, screen_height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    render_mode = new_render_mode;

    switch(render_mode) {
        case FW64_RENDERER_MODE_ORTHO2D:
            sprite_renderer.begin(camera);
            break;

        case FW64_RENDERER_MODE_TRIANGLES:
            mesh_renderer.render_mode = fw64Primitive::Mode::Triangles;
            mesh_renderer.begin(camera);
            break;

        case FW64_RENDERER_MODE_LINES:
            mesh_renderer.render_mode = fw64Primitive::Mode::Lines;
            mesh_renderer.begin(camera);
            break;

        default:
            break;
    }
}

void fw64Renderer::end(fw64RendererFlags flags) {
    switch(render_mode) {
        case FW64_RENDERER_MODE_ORTHO2D:
            sprite_renderer.end();
            break;

        case FW64_RENDERER_MODE_TRIANGLES:
        case FW64_RENDERER_MODE_LINES:
            mesh_renderer.end();
            break;

        default:
            break;
    }

    if (post_draw_callback) {
        framebuffer_write_texture.texture.image.clear();
        post_draw_callback(&framebuffer_write_texture, post_draw_callback_arg);
        framebuffer_write_texture.texture.image.updateGlImage();
        sprite_renderer.drawPixelTexture(framebuffer_write_texture.texture);
    }

    if ((flags & FW64_RENDERER_FLAG_SWAP) == FW64_RENDERER_FLAG_SWAP){
        SDL_GL_SwapWindow(window);
    }

    camera = nullptr;
}

void fw64Renderer::setScreenSize(int width, int height) {
    screen_width = width;
    screen_height = height;

    sprite_renderer.setScreenSize(width, height);
    framebuffer_write_texture.texture.setSize(width, height);
}

// Public C interface

void fw64_renderer_set_clear_color(fw64Renderer* renderer, uint8_t r, uint8_t g, uint8_t b) {
    renderer->setClearColor( r / 255.0f, g / 255.0f, b /255.0f, 1.0f);
}

void fw64_renderer_begin(fw64Renderer* renderer, fw64Camera* camera, fw64RenderMode render_mode, fw64RendererFlags flags) {
    renderer->begin(camera, render_mode, flags);
}

void fw64_renderer_end(fw64Renderer* renderer, fw64RendererFlags flags) {
    renderer->end(flags);
}

void fw64_renderer_draw_static_mesh(fw64Renderer* renderer, fw64Transform* transform, fw64Mesh* mesh) {
    assert(renderer->render_mode == FW64_RENDERER_MODE_TRIANGLES || renderer->render_mode == FW64_RENDERER_MODE_LINES);
    renderer->mesh_renderer.drawStaticMesh(mesh, transform);
}

void fw64_renderer_draw_animated_mesh(fw64Renderer* renderer, fw64Mesh* mesh, fw64AnimationController* controller, fw64Transform* transform) {
    assert(renderer->render_mode == FW64_RENDERER_MODE_TRIANGLES || renderer->render_mode == FW64_RENDERER_MODE_LINES);
    renderer->mesh_renderer.drawAnimatedMesh(mesh, controller, transform);
}

void fw64_renderer_draw_sprite(fw64Renderer* renderer, fw64Texture* texture, int x, int y) {
    assert(renderer->render_mode == FW64_RENDERER_MODE_ORTHO2D);
    renderer->sprite_renderer.drawSprite(texture, static_cast<float>(x), static_cast<float>(y));
}

void fw64_renderer_draw_sprite_slice(fw64Renderer* renderer, fw64Texture* texture, int frame, int x, int y) {
    assert(renderer->render_mode == FW64_RENDERER_MODE_ORTHO2D);
    renderer->sprite_renderer.drawSpriteFrame(texture, frame, static_cast<float>(x), static_cast<float>(y), 1.0f, 1.0f);
}

void fw64_renderer_draw_sprite_slice_transform(fw64Renderer* renderer, fw64Texture* texture, int frame, int x, int y, float scale_x , float scale_y, float rotation) {
    (void)rotation;
    assert(renderer->render_mode == FW64_RENDERER_MODE_ORTHO2D);
    renderer->sprite_renderer.drawSpriteFrame(texture, frame, static_cast<float>(x), static_cast<float>(y), scale_x, scale_y);
}

void fw64_renderer_draw_text(fw64Renderer* renderer, fw64Font* font, int x, int y, const char* text) {
    assert(renderer->render_mode == FW64_RENDERER_MODE_ORTHO2D);
    renderer->sprite_renderer.drawText(font, static_cast<float>(x), static_cast<float>(y), text, std::numeric_limits<uint32_t>::max());
}

void fw64_renderer_draw_text_count(fw64Renderer* renderer, fw64Font* font, int x, int y, const char* text, uint32_t count) {
    assert(renderer->render_mode == FW64_RENDERER_MODE_ORTHO2D);
    renderer->sprite_renderer.drawText(font, static_cast<float>(x), static_cast<float>(y), text, count);
}

void fw64_renderer_get_screen_size(fw64Renderer* renderer, IVec2* screen_size) {
    screen_size->x = renderer->screen_width;
    screen_size->y = renderer->screen_height;
}

fw64Camera* fw64_renderer_get_camera(fw64Renderer* renderer) {
    return renderer->camera;
}

void fw64_renderer_set_depth_testing_enabled(fw64Renderer* renderer, int enabled) {
    renderer->mesh_renderer.setDepthTestingEnabled(enabled);
}

int fw64_renderer_get_depth_testing_enabled(fw64Renderer* renderer) {
    return renderer->mesh_renderer.depthTestingEnabled();
}

void fw64_renderer_set_ambient_light_color(fw64Renderer* renderer, uint8_t r, uint8_t g, uint8_t b) {
    renderer->mesh_renderer.setAmbientLightColor(r, g, b);
}

void fw64_renderer_set_light_enabled(fw64Renderer* renderer, int index, int enabled) {
    renderer->mesh_renderer.setLightEnabled(index, enabled);
}

void fw64_renderer_set_light_direction(fw64Renderer* renderer, int index, float x, float y, float z) {
    renderer->mesh_renderer.setLightDirection(index, x, y, z);
}

void fw64_renderer_set_light_color(fw64Renderer* renderer, int index, uint8_t r, uint8_t g, uint8_t b) {
    renderer->mesh_renderer.setLightColor(index, r, g, b);
}

void fw64_renderer_set_anti_aliasing_enabled(fw64Renderer* renderer, int enabled) {
    renderer->anti_aliasing_enabled = static_cast<bool>(enabled);
}

int fw64_renderer_get_anti_aliasing_enabled(fw64Renderer* renderer) {
    return static_cast<int>(renderer->anti_aliasing_enabled);
}

void fw64_renderer_set_post_draw_callback(fw64Renderer* renderer, fw64RendererPostDrawFunc func, void* arg) {
    renderer->post_draw_callback = func;
    renderer->post_draw_callback_arg = arg;
}

void fw64_framebuffer_set_pixel(fw64Framebuffer* framebuffer, int x, int y, uint8_t r, uint8_t g, uint8_t b) {
    framebuffer->texture.image.setPixel(x, y, r, g, b);
}