#pragma once

/** \file renderer.h */

#include "animation_controller.h"
#include "camera.h"
#include "color.h"
#include "font.h"
#include "framebuffer.h"
#include "mesh.h"
#include "texture.h"
#include "vec2.h"

typedef enum {
    FW64_RENDERER_MODE_UNSET,
    FW64_RENDERER_MODE_TRIANGLES,
    FW64_RENDERER_MODE_LINES
} fw64RenderMode;

typedef enum {
    FW64_RENDERER_FLAG_NONE,
    FW64_RENDERER_FLAG_NOCLEAR = 0,
    FW64_RENDERER_FLAG_CLEAR_COLOR = 1,
    FW64_RENDERER_FLAG_CLEAR_DEPTH = 2,
    FW64_RENDERER_FLAG_CLEAR = FW64_RENDERER_FLAG_CLEAR_COLOR | FW64_RENDERER_FLAG_CLEAR_DEPTH,

    FW64_RENDERER_FLAG_NOSWAP = 0,
    FW64_RENDERER_FLAG_SWAP = 4,
} fw64RendererFlags;

#define FW64_RENDERER_MAX_LIGHT_COUNT 2

typedef struct fw64Renderer fw64Renderer;

typedef void(*fw64RendererPostDrawFunc)(fw64Framebuffer*, void*);

#ifdef __cplusplus
extern "C" {
#endif

void fw64_renderer_begin(fw64Renderer* renderer, fw64RenderMode render_mode, fw64RendererFlags flags);

/** 
 * Sets the currently active camera and the associated viewport.
 * This method should be called after \ref fw64_renderer_begin before rendering mesh primitives.
 */ 
void fw64_renderer_set_camera(fw64Renderer* renderer, fw64Camera* camera);

void fw64_renderer_set_clear_color(fw64Renderer* renderer, uint8_t r, uint8_t g, uint8_t b);
void fw64_renderer_end(fw64Renderer* renderer, fw64RendererFlags flags);

void fw64_renderer_set_depth_testing_enabled(fw64Renderer* renderer, int enabled);
int fw64_renderer_get_depth_testing_enabled(fw64Renderer* renderer);

void fw64_renderer_set_anti_aliasing_enabled(fw64Renderer* renderer, int enabled);
int fw64_renderer_get_anti_aliasing_enabled(fw64Renderer* renderer);

/** Draws a mesh with the supplied transform. */
void fw64_renderer_draw_static_mesh(fw64Renderer* renderer, fw64Transform* transform, fw64Mesh* mesh);

/** Draws an animated mesh with the supplied transform and animation state. */
void fw64_renderer_draw_animated_mesh(fw64Renderer* renderer, fw64Mesh* mesh, fw64AnimationController* controller, fw64Transform* transform);

/**
 * Draws all frames of a sprite.
 * Useful for rendering a texture that does not fit into a single frame.
 * The x, y position specifies the top left position of the sprite.
 */
void fw64_renderer_draw_sprite(fw64Renderer* renderer, fw64Texture* texture, int x, int y);

/**
 * Draws a single sprite frame.
 * The x, y position specifies the top left position of the sprite.
 * Note: the rotation value is currently ignored.
 */
void fw64_renderer_draw_sprite_slice(fw64Renderer* renderer, fw64Texture* texture, int frame, int x, int y);

/**
 * Draws a sprite frame with the supplied transform values.
 * The x, y position specifies the top left position of the sprite.
 * Note: the rotation value is currently ignored.
 */
void fw64_renderer_draw_sprite_slice_transform(fw64Renderer* renderer, fw64Texture* texture, int frame, int x, int y, float scale_x , float scale_y, float rotation);
void fw64_renderer_draw_text(fw64Renderer* renderer, fw64Font* font, int x, int y, const char* text);
void fw64_renderer_draw_text_count(fw64Renderer* renderer, fw64Font* font, int x, int y, const char* text, uint32_t count);

void fw64_renderer_get_screen_size(fw64Renderer* renderer, IVec2* screen_size);
IVec2 fw64_renderer_get_viewport_size(fw64Renderer* renderer, fw64Camera* camera);
fw64Camera* fw64_renderer_get_camera(fw64Renderer* renderer);

void fw64_renderer_set_ambient_light_color(fw64Renderer* renderer, uint8_t r, uint8_t g, uint8_t b);
void fw64_renderer_set_light_enabled(fw64Renderer* renderer, int index, int enabled);
void fw64_renderer_set_light_direction(fw64Renderer* renderer, int index, float x, float y, float z);
void fw64_renderer_set_light_color(fw64Renderer* renderer, int index, uint8_t r, uint8_t g, uint8_t b);

void fw64_renderer_set_post_draw_callback(fw64Renderer* renderer, fw64RendererPostDrawFunc func, void* arg);

#ifdef __cplusplus
}
#endif
