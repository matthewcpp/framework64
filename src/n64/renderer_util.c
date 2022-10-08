#include "framework64/n64/renderer.h"

static Mtx overlay_matrix;

static Vtx fullscreen_overlay_triangle[] = {
    {-4, 3, 0, 0, 0, 0, 255, 255, 255, 255},
    {4, 3, 0, 0, 0, 0, 255, 255, 255, 255},
    {4, -3, 0, 0, 0, 0, 255, 255, 255, 255},
    {-4, -3, 0, 0, 0, 0, 255, 255, 255, 255},
};

static Gfx fullscreen_overlay_dl[] = {
    gsDPSetRenderMode(G_RM_XLU_SURF, G_RM_XLU_SURF),
    gsSPClearGeometryMode(G_CULL_BACK),
    gsDPSetCombineMode(G_CC_PRIMITIVE,G_CC_PRIMITIVE),
    gsSPForceMatrix(OS_K0_TO_PHYSICAL(&(overlay_matrix))),
    gsSPVertex(&fullscreen_overlay_triangle[0], 4, 0),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsDPPipeSync(),
    gsSPEndDisplayList()
};

void fw64_renderer_util_fullscreen_overlay(fw64Renderer* renderer, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    guMtxIdent(&overlay_matrix);
    gDPSetPrimColor(renderer->display_list++, 0xFFFF, 0xFFFF, r, g, b, a);
    gSPDisplayList(renderer->display_list++, &fullscreen_overlay_dl[0]);
} 

void fw64_renderer_util_clear_viewport(fw64Renderer* renderer, fw64Camera* camera, fw64RendererFlags flags) {
    int x = (int)(renderer->screen_size.x * camera->viewport_pos.x);
    int y = (int)(renderer->screen_size.y * camera->viewport_pos.y);
    int width = (int)(renderer->screen_size.x * camera->viewport_size.x);
    int height = (int)(renderer->screen_size.y * camera->viewport_size.y);

    u32 cycle_type = fw64_renderer_get_fog_enabled(renderer) ? G_CYC_2CYCLE : G_CYC_1CYCLE;

    gDPSetRenderMode(renderer->display_list++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
    fw64_n64_renderer_clear_rect(renderer, x, y, width, height, flags);
    gDPSetCycleType(renderer->display_list++, cycle_type); 
    renderer->render_mode = FW64_RENDERER_MODE_UNSET;
}