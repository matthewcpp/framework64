#pragma once

#include "framework64/renderer.h"
#include "framework64/color.h"
#include "framework64/mesh_instance.h"
#include "framework64/n64_libultra/display_list.h"
#include "framework64/n64_libultra/render_pass.h"

#include <nusys.h>

/* The maximum length of the display list of one task  */
#define GFX_DLIST_LEN 4096

#define MAX_RENDERPASS_COUNT 16

struct fw64Renderer{
    // holds the current command insertion point of the display list
    Gfx* display_list;
    Gfx* display_list_start;

    // display list for drawing commands
    Gfx gfx_list[GFX_DLIST_LEN];

    u16 clear_color;

    fw64N64RendererFeature enabled_features;

    IVec2 screen_size;
    fw64PrimitiveMode primitive_mode;
    fw64ShadingMode shading_mode;

    fw64TextureState active_texture;
    Light empty_light; // this is used in the case where only an ambient light is set

    fw64RenderPass* renderpasses[MAX_RENDERPASS_COUNT];
    size_t renderpass_count;

    /** the index of the renderpass currently being drawn */
    size_t renderpass_index;
};

/** called by the n64_libultra engine which will render all the submitted renderpasses */
void fw64_n64_renderer_end_frame(fw64Renderer* renderer);
void fw64_n64_renderer_draw_renderpass(fw64Renderer* renderer, fw64RenderPass* renderpass);

void fw64_n64_renderer_init(fw64Renderer* renderer, int screen_width, int screen_height);

void fw64_n64_configure_fog(fw64Renderer* renderer, int enabled);

void fw64_n64_renderer_clear_rect(fw64Renderer* renderer, int x, int y, int width, int height, u16 clear_color, fw64ClearFlags flags);
void fw64_n64_renderer_clear_viewport(fw64Renderer* renderer, fw64Viewport* viewport, u16 clear_color, fw64ClearFlags flags);

/** Draws a mesh with the supplied transform. */
void fw64_renderer_draw_static_mesh(fw64Renderer* renderer, fw64MeshInstance* mesh_instance);

void fw64_renderer_draw_text(fw64Renderer* renderer, fw64Font* font, int x, int y, const char* text);
void fw64_renderer_draw_text_count(fw64Renderer* renderer, fw64Font* font, int x, int y, const char* text, uint32_t count);