#include "renderer.h"

#include "matrix.h"

#include <malloc.h>

Renderer* renderer_create(int screen_width, int screen_height) {
    Renderer* renderer = malloc(sizeof(Renderer));
    renderer->screen_size.x = screen_width;
    renderer->screen_size.y = screen_height;

    /*
      The viewport structure 
      The conversion from (-1,-1,-1)-(1,1,1).  The decimal part of 2-bit.
    */
    Vp view_port = {
      screen_width*2, screen_height*2, G_MAXZ/2, 0,	/* The scale factor  */
      screen_width*2, screen_height*2, G_MAXZ/2, 0,	/* Move  */
    };

    renderer->view_port = view_port;

    return renderer;
}

void renderer_init_rcp(Renderer* renderer) {
    /* Setting the RSP segment register  */
    gSPSegment(renderer->display_list++, 0, 0x0);  /* For the CPU virtual address  */

    /* Setting RSP  */
    gSPViewport(renderer->display_list++, &renderer->view_port);
    gSPClearGeometryMode(renderer->display_list++, 0xFFFFFFFF);
    gSPSetGeometryMode(renderer->display_list++, G_ZBUFFER | G_SHADE | G_SHADING_SMOOTH | G_CULL_BACK);
    gSPTexture(renderer->display_list++, 0, 0, 0, 0, G_OFF);

    /* Setting RDP  */
    gDPSetRenderMode(renderer->display_list++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
    gDPSetCombineMode(renderer->display_list++, G_CC_SHADE, G_CC_SHADE);
    gDPSetScissor(renderer->display_list++, G_SC_NON_INTERLACE, 0,0, renderer->screen_size.x,renderer->screen_size.y);
    gDPSetColorDither(renderer->display_list++, G_CD_BAYER);
}

void renderer_clear(Renderer* renderer) {
  /* Clear the Z-buffer  */
  gDPSetDepthImage(renderer->display_list++, OS_K0_TO_PHYSICAL(nuGfxZBuffer));
  gDPSetCycleType(renderer->display_list++, G_CYC_FILL);
  gDPSetColorImage(renderer->display_list++, G_IM_FMT_RGBA, G_IM_SIZ_16b,renderer->screen_size.x, OS_K0_TO_PHYSICAL(nuGfxZBuffer));
  gDPSetFillColor(renderer->display_list++,(GPACK_ZDZ(G_MAXFBZ,0) << 16 | GPACK_ZDZ(G_MAXFBZ,0)));
  gDPFillRectangle(renderer->display_list++, 0, 0, renderer->screen_size.x - 1, renderer->screen_size.y - 1);
  gDPPipeSync(renderer->display_list++);
  
    /* Clear the frame buffer  */
  gDPSetColorImage(renderer->display_list++, G_IM_FMT_RGBA, G_IM_SIZ_16b, renderer->screen_size.x, osVirtualToPhysical(nuGfxCfb_ptr));
  gDPSetFillColor(renderer->display_list++, (GPACK_RGBA5551(0, 0, 0, 1) << 16 | GPACK_RGBA5551(0, 0, 0, 1)));
  gDPFillRectangle(renderer->display_list++, 0, 0, renderer->screen_size.x - 1, renderer->screen_size.y - 1);
  gDPPipeSync(renderer->display_list++);
}

void renderer_begin(Renderer* renderer, Camera* camera) {
    // set the display list pointer to the beginning
    renderer->display_list = &renderer->gfx_list[0];

    renderer_init_rcp(renderer);
    renderer_clear(renderer);

    // sets the projection matrix (modelling set in individual draw calls)
    gSPMatrix(renderer->display_list++,OS_K0_TO_PHYSICAL(&(camera->projection)), G_MTX_PROJECTION|G_MTX_LOAD|G_MTX_NOPUSH);
    gSPPerspNormalize(renderer->display_list++, camera->perspNorm);

    gSPMatrix(renderer->display_list++,OS_K0_TO_PHYSICAL(&(camera->view)), G_MTX_MODELVIEW|G_MTX_LOAD|G_MTX_NOPUSH);

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

float entity_matrix[4][4];

void renderer_draw_static(Renderer* renderer, Entity* entity, const Gfx* static_display_list) {
    matrix_from_trs(entity_matrix, &entity->transform.position, &entity->transform.rotation, &entity->transform.scale);
    guMtxF2L(entity_matrix, &entity->dl_matrix);

    gSPMatrix(renderer->display_list++,OS_K0_TO_PHYSICAL(&(entity->dl_matrix)), G_MTX_MODELVIEW|G_MTX_MUL|G_MTX_PUSH);

    gSPDisplayList(renderer->display_list++, static_display_list);
    gSPPopMatrix(renderer->display_list++, G_MTX_MODELVIEW);
    gDPPipeSync(renderer->display_list++);
}

void renderer_get_screen_size(Renderer* renderer, IVec2* screen_size) {
  *screen_size = renderer->screen_size;
}
