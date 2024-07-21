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
    FW64_RENDERER_FLAG_NONE = 0,
    FW64_RENDERER_FLAG_NOCLEAR = 0,
    FW64_RENDERER_FLAG_CLEAR_COLOR = 1,
    FW64_RENDERER_FLAG_CLEAR_DEPTH = 2,
    FW64_RENDERER_FLAG_CLEAR = FW64_RENDERER_FLAG_CLEAR_COLOR | FW64_RENDERER_FLAG_CLEAR_DEPTH,
    FW64_RENDERER_FLAG_CLEAR_ALL = FW64_RENDERER_FLAG_CLEAR,

    FW64_RENDERER_FLAG_NOSWAP = 0,
    FW64_RENDERER_FLAG_SWAP = 4,
} fw64RendererFlags;

#define FW64_RENDERER_MAX_LIGHT_COUNT 2

typedef struct fw64Renderer fw64Renderer;


#ifdef __cplusplus
extern "C" {
#endif


void fw64_renderer_begin(fw64Renderer* renderer, fw64PrimitiveMode primitive_mode, fw64RendererFlags flags);

void fw64_renderer_submit_renderpass(fw64Renderer* renderer, fw64RenderPass* renderpass);

void fw64_renderer_set_clear_color(fw64Renderer* renderer, uint8_t r, uint8_t g, uint8_t b);
void fw64_renderer_end(fw64Renderer* renderer, fw64RendererFlags flags);

void fw64_renderer_set_depth_testing_enabled(fw64Renderer* renderer, int enabled);
int fw64_renderer_get_depth_testing_enabled(fw64Renderer* renderer);

void fw64_renderer_set_anti_aliasing_enabled(fw64Renderer* renderer, int enabled);
int fw64_renderer_get_anti_aliasing_enabled(fw64Renderer* renderer);

void fw64_renderer_set_ambient_light_color(fw64Renderer* renderer, uint8_t r, uint8_t g, uint8_t b);
void fw64_renderer_set_light_enabled(fw64Renderer* renderer, int index, int enabled);
void fw64_renderer_set_light_direction(fw64Renderer* renderer, int index, float x, float y, float z);
void fw64_renderer_set_light_color(fw64Renderer* renderer, int index, uint8_t r, uint8_t g, uint8_t b);


#ifdef __cplusplus
}
#endif
