#pragma once

/** \file renderer.h */

#include "camera.h"
#include "color.h"
#include "mesh.h"
#include "texture.h"
#include "vec2.h"
#include "font.h"

typedef enum {
    FW64_RENDERER_MODE_UNSET,
    FW64_RENDERER_MODE_TRIANGLES,
    FW64_RENDERER_MODE_LINES,
    FW64_RENDERER_MODE_ORTHO2D,
} fw64RenderMode;

typedef enum {
    FW64_RENDERER_FLAG_NONE,
    FW64_RENDERER_FLAG_NOCLEAR = 0,
    FW64_RENDERER_FLAG_CLEAR = 1,

    FW64_RENDERER_FLAG_NOSWAP = 0,
    FW64_RENDERER_FLAG_SWAP = 2,
} fw64RendererFlags;

#define FW64_RENDERER_MAX_LIGHT_COUNT 2

typedef struct fw64Renderer fw64Renderer;

#ifdef __cplusplus
extern "C" {
#endif

void fw64_renderer_begin(fw64Renderer* renderer, fw64Camera* camera, fw64RenderMode render_mode, fw64RendererFlags flags);
void fw64_renderer_set_clear_color(fw64Renderer* renderer, uint8_t r, uint8_t g, uint8_t b);
void fw64_renderer_end(fw64Renderer* renderer, fw64RendererFlags flags);

void fw64_renderer_set_depth_testing_enabled(fw64Renderer* renderer, int enabled);
int fw64_renderer_get_depth_testing_enabled(fw64Renderer* renderer);

void fw64_renderer_draw_static_mesh(fw64Renderer* renderer, fw64Transform* transform, fw64Mesh* mesh);

void fw64_renderer_draw_decal(fw64Renderer* renderer, fw64Transform* transform, fw64Mesh* mesh);

void fw64_renderer_draw_sprite(fw64Renderer* renderer, fw64Texture* texture, int x, int y);
void fw64_renderer_draw_sprite_slice(fw64Renderer* renderer, fw64Texture* texture, int frame, int x, int y);
void fw64_renderer_draw_text(fw64Renderer* renderer, fw64Font* font, int x, int y, const char* text);

void fw64_renderer_get_screen_size(fw64Renderer* renderer, IVec2* screen_size);
fw64Camera* fw64_renderer_get_camera(fw64Renderer* renderer);

void fw64_renderer_set_ambient_light_color(fw64Renderer* renderer, uint8_t r, uint8_t g, uint8_t b);
void fw64_renderer_set_light_enabled(fw64Renderer* renderer, int index, int enabled);
void fw64_renderer_set_light_direction(fw64Renderer* renderer, int index, float x, float y, float z);
void fw64_renderer_set_light_color(fw64Renderer* renderer, int index, uint8_t r, uint8_t g, uint8_t b);

#ifdef __cplusplus
}
#endif
