#pragma once

/** \file renderer.h */

#include "animation_controller.h"
#include "camera.h"
#include "color.h"
#include "font.h"
#include "mesh_instance.h"
#include "render_pass.h"
#include "sprite_batch.h"
#include "texture.h"
#include "vec2.h"

typedef enum {
    FW64_RENDERER_FLAG_NOSWAP = 0,
    FW64_RENDERER_FLAG_SWAP = 1,
} fw64RendererSwapFlags;

// this needs to be kept in sync with:
// - src/desktop/glsl/gouraud.vert.glsl
#define FW64_RENDERER_MAX_LIGHT_COUNT 3

typedef struct fw64Renderer fw64Renderer;


#ifdef __cplusplus
extern "C" {
#endif

void fw64_renderer_begin(fw64Renderer* renderer, fw64PrimitiveMode primitive_mode, fw64ClearFlags clear_flags);

void fw64_renderer_submit_renderpass(fw64Renderer* renderer, fw64RenderPass* renderpass);

void fw64_renderer_set_clear_color(fw64Renderer* renderer, uint8_t r, uint8_t g, uint8_t b);
void fw64_renderer_end(fw64Renderer* renderer, fw64RendererSwapFlags swap_flags);

void fw64_renderer_set_anti_aliasing_enabled(fw64Renderer* renderer, int enabled);
int fw64_renderer_get_anti_aliasing_enabled(fw64Renderer* renderer);

#ifdef __cplusplus
}
#endif
