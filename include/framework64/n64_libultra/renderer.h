#pragma once

#include "framework64/renderer.h"
#include "framework64/color.h"
#include "framework64/n64_libultra/fill_rect.h"
#include "framework64/n64_libultra/display_list.h"
#include "framework64/n64_libultra/render_pass.h"

#include <nusys.h>

/* The maximum length of the display list of one task  */
#define GFX_DLIST_LEN 4096

struct fw64Renderer{
    // holds the current command insertion point of the display list
    Gfx* display_list;
    Gfx* display_list_start;
    Mtx identity_matrix;

    // display list for drawing commands
    Gfx gfx_list[GFX_DLIST_LEN];

    fw64Camera* camera;

    u16 clear_color;

    fw64N64RendererFeature enabled_features;

    s32 fog_min;
    s32 fog_max;
    fw64ColorRGBA8 fog_color;

    IVec2 screen_size;
    fw64PrimitiveMode primitive_mode;
    fw64ShadingMode shading_mode;
    fw64RendererFlags flags;

    Lights2 lights;
    uint32_t active_light_mask;
    uint32_t starting_new_frame;

    fw64TextureState active_texture;
    N64FillRect fill_rect;
};

void fw64_n64_renderer_init(fw64Renderer* renderer, int screen_width, int screen_height);

void fw64_n64_configure_fog(fw64Renderer* renderer, int enabled);

void fw64_n64_renderer_clear_rect(fw64Renderer* renderer, int x, int y, int width, int height, u16 clear_color, fw64ClearFlags flags);
void fw64_n64_renderer_clear_viewport(fw64Renderer* renderer, fw64Viewport* viewport, u16 clear_color, fw64ClearFlags flags);