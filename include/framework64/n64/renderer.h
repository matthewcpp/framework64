#pragma once

#include "framework64/renderer.h"
#include "framework64/color.h"

#include <nusys.h>

/* The maximum length of the display list of one task  */
#define GFX_DLIST_LEN     2048

struct fw64Renderer{
    // holds the current command insertion point of the display list
    Gfx* display_list;
    Gfx* display_list_start;

    // display list for drawing commands
    Gfx gfx_list[GFX_DLIST_LEN];

    fw64Camera* camera;

    u16 fill_color;
    u16 clear_color;

    u32 depth_test_enabled;
    u32 aa_enabled;

    Vp view_port;
    IVec2 screen_size;
    fw64RenderMode render_mode;
    fw64ShadingMode shading_mode;
    fw64RendererFlags flags;

    Lights2 lights;
    uint32_t active_light_mask;

    fw64RendererPostDrawFunc post_draw_func;
    void* post_draw_func_arg;
};

void fw64_n64_renderer_init(fw64Renderer* renderer, int screen_width, int screen_height);

void fw64_n64_renderer_swap_func(fw64Renderer* renderer, NUScTask*gfxTaskPtr);

#define FW64_N64_OPA_ZB_FLAGS(renderer) ((renderer)->depth_test_enabled ? Z_CMP | Z_UPD : 0)
#define FW64_N64_OPA_AA_FLAGS(renderer) ((renderer)->aa_enabled ? AA_EN : 0)

/** based on G_RM_[AA]_[ZB]_TEX_EDGE macro definition. */
#define FW64_RENDER_MODE_3D_TEXTURED(renderer, cycle) \
	FW64_N64_OPA_AA_FLAGS((renderer)) | FW64_N64_OPA_ZB_FLAGS((renderer)) | IM_RD | CVG_DST_CLAMP |		\
	CVG_X_ALPHA | ALPHA_CVG_SEL | ZMODE_OPA | TEX_EDGE |	\
	GBL_c##cycle(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_A_MEM)

#define FW64_RM_3D_TEXTURED(renderer) FW64_RENDER_MODE_3D_TEXTURED(renderer, 1)
#define FW64_RM_3D_TEXTURED2(renderer) FW64_RENDER_MODE_3D_TEXTURED(renderer, 2)

/** based on G_RM_[AA]_[ZB]_OPA_SURF macro definition. */
#define FW64_RENDER_MODE_3D_OPAQUE_SHADED(renderer, cycle) \
	FW64_N64_OPA_AA_FLAGS((renderer)) | FW64_N64_OPA_ZB_FLAGS((renderer)) | IM_RD | CVG_DST_CLAMP |		\
	ZMODE_OPA | ALPHA_CVG_SEL |				\
	GBL_c##cycle(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_A_MEM)

#define FW64_RM_3D_OPAQUE_SHADED(renderer) FW64_RENDER_MODE_3D_OPAQUE_SHADED(renderer, 1)
#define FW64_RM_3D_OPAQUE_SHADED2(renderer) FW64_RENDER_MODE_3D_OPAQUE_SHADED(renderer, 2)

/** based on G_RM_[AA]_XLU_SURF macro definition. */

#define FW64_N64_XLU_AA_FLAGS(renderer) ((renderer)->aa_enabled ? (AA_EN | CVG_DST_WRAP | CLR_ON_CVG) : CVG_DST_FULL)

#define FW64_RENDER_MODE_TRANSLUCENT_SPRITE(renderer, cycle) \
	FW64_N64_XLU_AA_FLAGS(renderer) | IM_RD  | FORCE_BL |	ZMODE_OPA |		\
	GBL_c##cycle(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA)

#define FW64_RM_TRANSLUCENT_SPRITE(renderer) FW64_RENDER_MODE_TRANSLUCENT_SPRITE(renderer, 1)
#define FW64_RM_TRANSLUCENT_SPRITE2(renderer) FW64_RENDER_MODE_TRANSLUCENT_SPRITE(renderer, 2)

