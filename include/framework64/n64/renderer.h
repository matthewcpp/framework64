#pragma once

#include "framework64/renderer.h"
#include "framework64/color.h"
#include "framework64/n64/fill_rect.h"

#include <nusys.h>



/* The maximum length of the display list of one task  */
#define GFX_DLIST_LEN 4096

typedef enum {
    N64_RENDERER_FEATURE_NONE = 0,
    N64_RENDERER_FEATURE_AA = 1,
    N64_RENDERER_FEATURE_DEPTH_TEST = 2,
    N64_RENDERER_FEATURE_FOG = 4,
    N64_RENDERER_FEATURE_COLOR_INDEX_MODE = 8
} fw64N64RendererFeature;

struct fw64Renderer{
    // holds the current command insertion point of the display list
    Gfx* display_list;
    Gfx* display_list_start;

    // display list for drawing commands
    Gfx gfx_list[GFX_DLIST_LEN];

    fw64Camera* camera;

    u16 clear_color;

    fw64N64RendererFeature enabled_features;

    s32 fog_min;
    s32 fog_max;
    fw64ColorRGBA8 fog_color;

    IVec2 screen_size;
    IVec2 viewport_screen_pos;
    fw64PrimitiveMode primitive_mode;
    fw64ShadingMode shading_mode;
    fw64RendererFlags flags;

    Lights2 lights;
    uint32_t active_light_mask;
    uint32_t starting_new_frame;

    fw64RendererPostDrawFunc post_draw_func;
    void* post_draw_func_arg;
    fw64Texture* active_texture;
    int active_texture_frame;
    uint32_t active_palette;
    N64FillRect fill_rect;
};

void fw64_n64_renderer_init(fw64Renderer* renderer, int screen_width, int screen_height);

void fw64_n64_configure_fog(fw64Renderer* renderer, int enabled);

void fw64_n64_renderer_swap_func(fw64Renderer* renderer, NUScTask* gfxTaskPtr);

void fw64_n64_renderer_clear_rect(fw64Renderer* renderer, int x, int y, int width, int height, fw64RendererFlags flags);
