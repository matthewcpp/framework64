#include "renderer.h"

#include "malloc.h"

Renderer* renderer_create() {
    Renderer* renderer = malloc(sizeof(Renderer));
    return renderer;
}

/*
  The viewport structure 
  The conversion from (-1,-1,-1)-(1,1,1).  The decimal part of 2-bit.
 */
static Vp view_port = {
    SCREEN_WD*2, SCREEN_HT*2, G_MAXZ/2, 0,	/* The scale factor  */
    SCREEN_WD*2, SCREEN_HT*2, G_MAXZ/2, 0,	/* Move  */
};

/*
  The initialization of RDP 
*/
Gfx setup_rdp_display_list[] = {
  gsDPSetRenderMode(G_RM_OPA_SURF, G_RM_OPA_SURF2),
  gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
  gsDPSetScissor(G_SC_NON_INTERLACE, 0,0, SCREEN_WD,SCREEN_HT),
  gsDPSetColorDither(G_CD_BAYER),
  gsSPEndDisplayList(),
};

/*
  The initialization of RSP 
*/
Gfx setup_rsp_display_list[] = {
  gsSPViewport(&view_port),
  gsSPClearGeometryMode(0xFFFFFFFF),
  gsSPSetGeometryMode(G_ZBUFFER | G_SHADE | G_SHADING_SMOOTH | G_CULL_BACK),
  gsSPTexture(0, 0, 0, 0, G_OFF),
  gsSPEndDisplayList(),
};

void renderer_init_rcp(Renderer* renderer) {
    /* Setting the RSP segment register  */
    gSPSegment(renderer->display_list++, 0, 0x0);  /* For the CPU virtual address  */

    /* Setting RSP  */
    gSPDisplayList(renderer->display_list++, OS_K0_TO_PHYSICAL(setup_rsp_display_list));

    /* Setting RDP  */
    gSPDisplayList(renderer->display_list++, OS_K0_TO_PHYSICAL(setup_rdp_display_list));
}

void renderer_clear(Renderer* renderer) {
  /* Clear the Z-buffer  */
  gDPSetDepthImage(renderer->display_list++, OS_K0_TO_PHYSICAL(nuGfxZBuffer));
  gDPSetCycleType(renderer->display_list++, G_CYC_FILL);
  gDPSetColorImage(renderer->display_list++, G_IM_FMT_RGBA, G_IM_SIZ_16b,SCREEN_WD, OS_K0_TO_PHYSICAL(nuGfxZBuffer));
  gDPSetFillColor(renderer->display_list++,(GPACK_ZDZ(G_MAXFBZ,0) << 16 | GPACK_ZDZ(G_MAXFBZ,0)));
  gDPFillRectangle(renderer->display_list++, 0, 0, SCREEN_WD-1, SCREEN_HT-1);
  gDPPipeSync(renderer->display_list++);
  
    /* Clear the frame buffer  */
  gDPSetColorImage(renderer->display_list++, G_IM_FMT_RGBA, G_IM_SIZ_16b, SCREEN_WD, osVirtualToPhysical(nuGfxCfb_ptr));
  gDPSetFillColor(renderer->display_list++, (GPACK_RGBA5551(0, 0, 0, 1) << 16 | GPACK_RGBA5551(0, 0, 0, 1)));
  gDPFillRectangle(renderer->display_list++, 0, 0, SCREEN_WD-1, SCREEN_HT-1);
  gDPPipeSync(renderer->display_list++);
}

void renderer_begin(Renderer* renderer, Camera* camera) {
    // set the display list pointer to the beginning
    renderer->display_list = &renderer->gfx_list[0];

    renderer_init_rcp(renderer);
    renderer_clear(renderer);

    // sets the projection matrix (modelling set in individual draw calls)
    gSPMatrix(renderer->display_list++,OS_K0_TO_PHYSICAL(&(camera->projection)), G_MTX_PROJECTION|G_MTX_LOAD|G_MTX_NOPUSH);

    // set drawing mode to shaded
    gDPSetCycleType(renderer->display_list++,G_CYC_1CYCLE);
    gDPSetRenderMode(renderer->display_list++,G_RM_AA_OPA_SURF, G_RM_AA_OPA_SURF2);
    gSPClearGeometryMode(renderer->display_list++,0xFFFFFFFF);
    gSPSetGeometryMode(renderer->display_list++,G_SHADE| G_SHADING_SMOOTH);

    gDPPipeSync(renderer->display_list++);
}

void renderer_end(Renderer* renderer) {
    gDPFullSync(renderer->display_list++);
    gSPEndDisplayList(renderer->display_list++);

    /* Activate the task and switch display buffers.  */
    nuGfxTaskStart(&renderer->gfx_list[0], (s32)(renderer->display_list - renderer->gfx_list) * sizeof (Gfx), NU_GFX_UCODE_F3DEX , NU_SC_SWAPBUFFER);
}

void renderer_draw_static(Renderer* renderer, Dynamic* dynamic, const Gfx* static_display_list) {
    gSPMatrix(renderer->display_list++,OS_K0_TO_PHYSICAL(&(dynamic->translate)), G_MTX_MODELVIEW|G_MTX_LOAD|G_MTX_NOPUSH);
    gSPMatrix(renderer->display_list++,OS_K0_TO_PHYSICAL(&(dynamic->modeling)), G_MTX_MODELVIEW|G_MTX_MUL|G_MTX_NOPUSH);

    gSPDisplayList(renderer->display_list++, static_display_list);
    gDPPipeSync(renderer->display_list++);
}