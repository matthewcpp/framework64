#ifndef FW64_RENDERER_H
#define FW64_RENDERER_H

#include "camera.h"
#include "color.h"
#include "mesh.h"
#include "rect.h"
#include "texture.h"
#include "vec2.h"
#include "font.h"

typedef enum {
    FW64_RENDERER_MODE_UNSET,
    FW64_RENDERER_MODE_TRIANGLES,
    FW64_RENDERER_MODE_LINES,
    FW64_RENDERER_MODE_RECTANGLES,
    FW64_RENDERER_MODE_SPRITES
} fw64RenderMode;

typedef enum {
    FW64_RENDERER_FLAG_NONE,
    FW64_RENDERER_FLAG_NOCLEAR = 0,
    FW64_RENDERER_FLAG_CLEAR = 1,

    FW64_RENDERER_FLAG_NOSWAP = 0,
    FW64_RENDERER_FLAG_SWAP = 2,
} fw64RendererFlags;

typedef struct fw64Renderer fw64Renderer;

#ifdef __cplusplus
extern "C" {
#endif

void fw64_renderer_begin(fw64Renderer* renderer, fw64Camera* camera, fw64RenderMode render_mode, fw64RendererFlags flags);
void fw64_renderer_set_clear_color(fw64Renderer* renderer, uint8_t r, uint8_t g, uint8_t b);
void fw64_renderer_end(fw64Renderer* renderer, fw64RendererFlags flags);

void fw64_renderer_draw_static_mesh(fw64Renderer* renderer, fw64Transform* transform, fw64Mesh* mesh);

void fw64_renderer_set_fill_color(fw64Renderer* renderer, Color* color);
void fw64_renderer_set_fill_mode(fw64Renderer* renderer);
void fw64_renderer_draw_filled_rect(fw64Renderer* renderer, IRect* rect);

void fw64_renderer_draw_sprite(fw64Renderer* renderer, fw64Texture* texture, int x, int y);
void fw64_renderer_draw_sprite_slice(fw64Renderer* renderer, fw64Texture* texture, int frame, int x, int y);
void fw64_renderer_draw_text(fw64Renderer* renderer, fw64Font* font, int x, int y, const char* text);

void fw64_renderer_get_screen_size(fw64Renderer* renderer, IVec2* screen_size);

#ifdef __cplusplus
}
#endif

#endif