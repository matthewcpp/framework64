#include "ultra/renderer.h"

#include "ultra/matrix.h"

#include <nusys.h>

#include <malloc.h>

void renderer_init(Renderer* renderer, int screen_width, int screen_height) {
    nuGfxInit(); // starts nusys graphics

    renderer->screen_size.x = screen_width;
    renderer->screen_size.y = screen_height;
    renderer->display_list = NULL;

    renderer->fill_color.r = 255;
    renderer->fill_color.g = 255;
    renderer->fill_color.b = 255;

    /*
      The viewport structure 
      The conversion from (-1,-1,-1)-(1,1,1).  The decimal part of 2-bit.
    */
    Vp view_port = {
      screen_width*2, screen_height*2, G_MAXZ/2, 0,	/* The scale factor  */
      screen_width*2, screen_height*2, G_MAXZ/2, 0,	/* Move  */
    };

    renderer->view_port = view_port;
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

static void renderer_clear_z_buffer(Renderer* renderer) {
    gDPSetDepthImage(renderer->display_list++, OS_K0_TO_PHYSICAL(nuGfxZBuffer));
    gDPSetCycleType(renderer->display_list++, G_CYC_FILL);
    gDPSetColorImage(renderer->display_list++, G_IM_FMT_RGBA, G_IM_SIZ_16b,renderer->screen_size.x, OS_K0_TO_PHYSICAL(nuGfxZBuffer));
    gDPSetFillColor(renderer->display_list++,(GPACK_ZDZ(G_MAXFBZ,0) << 16 | GPACK_ZDZ(G_MAXFBZ,0)));
    gDPFillRectangle(renderer->display_list++, 0, 0, renderer->screen_size.x - 1, renderer->screen_size.y - 1);
    gDPPipeSync(renderer->display_list++);
}

void renderer_clear(Renderer* renderer) {
    /* Clear the Z-buffer  */
    renderer_clear_z_buffer(renderer);

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
    gDPSetRenderMode(renderer->display_list++,G_RM_AA_ZB_OPA_SURF, G_RM_AA_ZB_OPA_SURF2);
    gSPClearGeometryMode(renderer->display_list++,0xFFFFFFFF);
    gSPSetGeometryMode(renderer->display_list++,G_SHADE| G_SHADING_SMOOTH | G_ZBUFFER);

    // need this for drawing 3d
    //gDPSetTexturePersp(renderer->display_list++, G_TP_PERSP);

    gDPPipeSync(renderer->display_list++);
}

float entity_matrix[4][4];

void renderer_entity_start(Renderer* renderer, Entity* entity){
    matrix_from_trs((float*)entity_matrix, &entity->transform.position, &entity->transform.rotation, &entity->transform.scale);
    guMtxF2L(entity_matrix, &entity->dl_matrix);

    gSPMatrix(renderer->display_list++,OS_K0_TO_PHYSICAL(&(entity->dl_matrix)), G_MTX_MODELVIEW|G_MTX_MUL|G_MTX_PUSH);
}

void renderer_entity_end(Renderer* renderer) {
    gSPPopMatrix(renderer->display_list++, G_MTX_MODELVIEW);
    gDPPipeSync(renderer->display_list++);
}

void renderer_get_screen_size(Renderer* renderer, IVec2* screen_size) {
    *screen_size = renderer->screen_size;
}

void renderer_begin_2d(Renderer* renderer) {
    gSPClearGeometryMode(renderer->display_list++, 0xFFFFFFFF);
    gSPTexture(renderer->display_list++, 0, 0, 0, 0, G_OFF);
    gDPSetCycleType(renderer->display_list++, G_CYC_1CYCLE);
    gDPSetScissor(renderer->display_list++, G_SC_NON_INTERLACE, 0, 0, renderer->screen_size.x, renderer->screen_size.y);
    gDPSetCombineKey(renderer->display_list++, G_CK_NONE);
    gDPSetAlphaCompare(renderer->display_list++, G_AC_NONE);
    gDPSetRenderMode(renderer->display_list++, G_RM_NOOP, G_RM_NOOP2);
    gDPSetColorDither(renderer->display_list++, G_CD_DISABLE);
    gDPPipeSync(renderer->display_list++);
}

void renderer_end(Renderer* renderer) {
    gDPFullSync(renderer->display_list++);
    gSPEndDisplayList(renderer->display_list++);

    nuGfxTaskStart(&renderer->gfx_list[0], (s32)(renderer->display_list - renderer->gfx_list) * sizeof (Gfx), NU_GFX_UCODE_F3DEX , NU_SC_SWAPBUFFER);

    renderer->display_list = NULL;
}

void renderer_set_fill_color(Renderer* renderer, Color* color) {
    if (renderer->display_list) {
        gDPSetFillColor(renderer->display_list++, (GPACK_RGBA5551(color->r, color->g, color->b, 255) << 16 | GPACK_RGBA5551(color->r, color->g, color->b, 255)));
    }

    renderer->fill_color = *color;
}

void renderer_set_fill_mode(Renderer* renderer) {
    gDPSetFillColor(renderer->display_list++, (GPACK_RGBA5551(renderer->fill_color.r, renderer->fill_color.g, renderer->fill_color.b, 255) << 16 | GPACK_RGBA5551(renderer->fill_color.r, renderer->fill_color.g, renderer->fill_color.b, 255)));
    gDPSetCycleType(renderer->display_list++, G_CYC_FILL);
    //TODO: do i need to sync pipe here?
}

void renderer_draw_filled_rect(Renderer* renderer, IRect* rect) {
    
    gDPFillRectangle(renderer->display_list++, rect->x, rect->y, rect->x + rect->width, rect->y + rect->height);
    gDPPipeSync(renderer->display_list++);
}

void renderer_set_sprite_mode(Renderer* renderer){
    gDPSetCycleType(renderer->display_list++, G_CYC_1CYCLE);
    gDPSetCombineMode(renderer->display_list++, G_CC_DECALRGBA, G_CC_DECALRGBA);
    gDPSetRenderMode(renderer->display_list++, G_RM_AA_TEX_EDGE, G_RM_AA_TEX_EDGE);
    gDPSetTexturePersp(renderer->display_list++, G_TP_NONE);
    //TODO: do i need to sync pipe here?
}

void renderer_draw_sprite_slice(Renderer* renderer, ImageSprite* sprite, int frame, int x, int y) {
    int slice_width = image_sprite_get_slice_width(sprite);
    int slice_height = image_sprite_get_slice_height(sprite);

    int top_left_x = (frame % sprite->hslices) * slice_width;
    int top_left_y = (frame / sprite->hslices) * slice_height;

    gDPLoadTextureBlock(renderer->display_list++, sprite->slices[frame], G_IM_FMT_RGBA, G_IM_SIZ_16b, slice_width, slice_height, 0, 
        G_TX_CLAMP, G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

    gDPLoadSync(renderer->display_list++);

    gSPTextureRectangle(renderer->display_list++, 
            x << 2, y << 2, 
            (x + slice_width) << 2, (y + slice_height) << 2,
            G_TX_RENDERTILE, 
            0 << 5, 0 << 5, 
            1 << 10, 1 << 10);

    gDPPipeSync(renderer->display_list++);
}

void renderer_draw_sprite(Renderer* renderer, ImageSprite* sprite, int x, int y) {
    int slice_width = image_sprite_get_slice_width(sprite);
    int slice_height = image_sprite_get_slice_height(sprite);
    int slice = 0;

    for (uint8_t row = 0; row < sprite->hslices; row++ ) {
        int draw_y = y + row * slice_height;
        for (uint8_t col = 0; col < sprite->vslices; col++) {
            int draw_x = x + slice_width * col;

            renderer_draw_sprite_slice(renderer, sprite, slice++, draw_x, draw_y);
        }
    }
}

void renderer_draw_text_sprite(Renderer* renderer, TextSprite* text_sprite, int x, int y) {
    gDPLoadTextureBlock(renderer->display_list++, text_sprite->data, G_IM_FMT_RGBA, G_IM_SIZ_16b, text_sprite->allocated_width, text_sprite->text_height, 0, 
        G_TX_CLAMP, G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

    gDPLoadSync(renderer->display_list++);

        gSPTextureRectangle(renderer->display_list++, 
            x << 2, y << 2, 
            (x + text_sprite->allocated_width) << 2, (y + text_sprite->text_height) << 2,
            G_TX_RENDERTILE, 
            0 << 5, 0 << 5, 
            1 << 10, 1 << 10);

    gDPPipeSync(renderer->display_list++);
}
