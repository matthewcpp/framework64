#include "framework64/renderer.h"

#include "framework64/matrix.h"

#include <nusys.h>

#include <malloc.h>
#include <string.h>

void renderer_init(Renderer* renderer, int screen_width, int screen_height) {
    renderer->screen_size.x = screen_width;
    renderer->screen_size.y = screen_height;
    renderer->display_list = NULL;

    renderer->fill_color = GPACK_RGBA5551(255, 255, 255, 1);
    renderer->clear_color = GPACK_RGBA5551(0, 0, 0, 1);

    /*
      The viewport structure 
      The conversion from (-1,-1,-1)-(1,1,1).  The decimal part of 2-bit.
    */
    Vp view_port = {
      screen_width*2, screen_height*2, G_MAXZ/2, 0,	/* The scale factor  */
      screen_width*2, screen_height*2, G_MAXZ/2, 0,	/* Move  */
    };

    renderer->view_port = view_port;
    renderer->render_mode = RENDERER_MODE_UNSET;
    renderer->shading_mode = SHADING_MODE_UNSET;
}

void renderer_set_clear_color(Renderer* renderer, Color* clear_color) {
    renderer->clear_color = GPACK_RGBA5551(clear_color->r, clear_color->g, clear_color->b, 1);
}

Gfx _rdp_init_static_dl[] = {
    gsDPSetTextureLOD(G_TL_TILE),
    gsDPSetTextureLUT(G_TT_NONE),
    gsDPSetTextureDetail(G_TD_CLAMP),
    gsDPSetTexturePersp(G_TP_PERSP),
    gsDPSetTextureFilter(G_TF_BILERP),
    gsDPSetTextureConvert(G_TC_FILT),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsDPSetCombineKey(G_CK_NONE),
    gsDPSetAlphaCompare(G_AC_NONE),
    gsDPSetRenderMode(G_RM_OPA_SURF, G_RM_OPA_SURF2),
    gsDPSetColorDither(G_CD_DISABLE),
    gsSPEndDisplayList()
};

void renderer_init_rdp(Renderer* renderer) {
    gDPSetCycleType(renderer->display_list++, G_CYC_1CYCLE);
    gDPSetScissor(renderer->display_list++,G_SC_NON_INTERLACE, 0, 0, renderer->screen_size.x, renderer->screen_size.y);
    gSPDisplayList(renderer->display_list++, _rdp_init_static_dl);
    gDPSetDepthImage(renderer->display_list++,nuGfxZBuffer);
    gDPPipeSync(renderer->display_list++);
}


void renderer_init_rsp(Renderer* renderer) {
    gSPViewport(renderer->display_list++, &renderer->view_port);
    gSPClearGeometryMode(renderer->display_list++, G_SHADE | G_SHADING_SMOOTH | G_CULL_BOTH | G_FOG | G_LIGHTING | G_TEXTURE_GEN | G_TEXTURE_GEN_LINEAR | G_LOD);
    gSPTexture(renderer->display_list++, 0, 0, 0, 0, G_OFF);
    gSPSetGeometryMode(renderer->display_list++, G_ZBUFFER | G_SHADE | G_SHADING_SMOOTH);
}

void renderer_clear_z_buffer(Renderer* renderer) {
    //gDPSetDepthImage(renderer->display_list++, OS_K0_TO_PHYSICAL(nuGfxZBuffer));
    gDPSetColorImage(renderer->display_list++, G_IM_FMT_RGBA, G_IM_SIZ_16b,renderer->screen_size.x, OS_K0_TO_PHYSICAL(nuGfxZBuffer));
    gDPPipeSync(renderer->display_list++);
    gDPSetCycleType(renderer->display_list++, G_CYC_FILL);
    
    gDPSetFillColor(renderer->display_list++,(GPACK_ZDZ(G_MAXFBZ,0) << 16 | GPACK_ZDZ(G_MAXFBZ,0)));
    gDPFillRectangle(renderer->display_list++, 0, 0, renderer->screen_size.x - 1, renderer->screen_size.y - 1);
    gDPPipeSync(renderer->display_list++);
}

void renderer_clear_frame_buffer(Renderer* renderer) {
    gDPSetCycleType(renderer->display_list++, G_CYC_FILL);
    gDPSetColorImage(renderer->display_list++, G_IM_FMT_RGBA, G_IM_SIZ_16b, renderer->screen_size.x, OS_K0_TO_PHYSICAL(nuGfxCfb_ptr));
    gDPSetFillColor(renderer->display_list++, (renderer->clear_color << 16 | renderer->clear_color));
    gDPFillRectangle(renderer->display_list++, 0, 0, renderer->screen_size.x - 1, renderer->screen_size.y - 1);
    gDPPipeSync(renderer->display_list++);
    gDPSetCycleType(renderer->display_list++, G_CYC_1CYCLE); 
}

void renderer_begin(Renderer* renderer, Camera* camera, RenderMode render_mode, RendererFlags flags) {
    renderer->render_mode = render_mode;
    renderer->camera = camera;

    renderer->display_list = &renderer->gfx_list[0];
    renderer->display_list_start = renderer->display_list;

    gSPSegment(renderer->display_list++, 0, 0x0);
    renderer_init_rdp(renderer);
    renderer_init_rsp(renderer);

    if (flags & RENDERER_FLAG_CLEAR) {
        renderer_clear_z_buffer(renderer);
        renderer_clear_frame_buffer(renderer);
    }

    switch (renderer->render_mode) {
        case RENDERER_MODE_TRIANGLES:
            gDPSetRenderMode(renderer->display_list++, G_RM_AA_ZB_OPA_SURF, G_RM_AA_ZB_OPA_SURF2);
        break;

        case RENDERER_MODE_LINES:
            gDPSetRenderMode(renderer->display_list++,  G_RM_AA_ZB_XLU_LINE,  G_RM_AA_ZB_XLU_LINE2);
        break;

        case RENDERER_MODE_RECTANGLES:
            gSPClearGeometryMode(renderer->display_list++, 0xFFFFFFFF);
        break;
    }

    // sets the projection matrix (modelling set in individual draw calls)
    gSPMatrix(renderer->display_list++,OS_K0_TO_PHYSICAL(&(camera->projection)), G_MTX_PROJECTION|G_MTX_LOAD|G_MTX_NOPUSH);
    gSPPerspNormalize(renderer->display_list++, camera->perspNorm);
    gSPMatrix(renderer->display_list++,OS_K0_TO_PHYSICAL(&(camera->view)), G_MTX_MODELVIEW|G_MTX_LOAD|G_MTX_NOPUSH);
    
    gDPPipeSync(renderer->display_list++);
    
}

void renderer_end(Renderer* renderer, RendererFlags flags) {
    gDPFullSync(renderer->display_list++);
    gSPEndDisplayList(renderer->display_list++);

    renderer->shading_mode = SHADING_MODE_UNSET;

    nuGfxTaskStart(renderer->display_list_start, 
        (s32)(renderer->display_list - renderer->display_list_start) * sizeof (Gfx), 
        (renderer->render_mode == RENDERER_MODE_LINES) ? NU_GFX_UCODE_L3DEX2 : NU_GFX_UCODE_F3DEX, 
        (flags & RENDERER_FLAG_SWAP) ? NU_SC_SWAPBUFFER : NU_SC_NOSWAPBUFFER);
}

static void renderer_set_shading_mode(Renderer* renderer, ShadingMode shading_mode) {
    if (shading_mode == renderer->shading_mode) return;

    renderer->shading_mode = shading_mode;

    switch(renderer->shading_mode) {
        case SHADING_MODE_GOURAUD:
            gSPSetGeometryMode(renderer->display_list++, G_LIGHTING)
            gDPSetCombineMode(renderer->display_list++, G_CC_SHADE, G_CC_SHADE);
            break;

        case SHADING_MODE_GOURAUD_TEXTURED:
            gSPSetGeometryMode(renderer->display_list++, G_LIGHTING)
            gDPSetCombineMode(renderer->display_list++,G_CC_MODULATERGBA , G_CC_MODULATERGBA );
            gDPSetRenderMode(renderer->display_list++, G_RM_AA_ZB_TEX_EDGE, G_RM_AA_ZB_TEX_EDGE2);
            gSPTexture(renderer->display_list++, 0x8000, 0x8000, 0, 0, G_ON );
            gDPSetTexturePersp(renderer->display_list++, G_TP_PERSP);
            gDPSetTextureFilter(renderer->display_list++,G_TF_POINT);
        break;

        case SHADING_MODE_SPRITE:
            gDPSetCombineMode(renderer->display_list++, G_CC_DECALRGBA, G_CC_DECALRGBA);
            gDPSetRenderMode(renderer->display_list++, G_RM_AA_TEX_EDGE, G_RM_AA_TEX_EDGE);
            gDPSetTexturePersp(renderer->display_list++, G_TP_NONE);
            gDPPipeSync(renderer->display_list++);
            break;
        default:
            break;
    }
}

float entity_matrix[4][4];

static void renderer_set_transform(Renderer* renderer, Transform* transform, Mtx* dl_matrix) {
    matrix_from_trs((float*)entity_matrix, &transform->position, &transform->rotation, &transform->scale);
    guMtxF2L(entity_matrix, dl_matrix);

    gSPMatrix(renderer->display_list++,OS_K0_TO_PHYSICAL(dl_matrix), G_MTX_MODELVIEW|G_MTX_MUL|G_MTX_PUSH);
}

static void renderer_pop_transform(Renderer* renderer) {
    gSPPopMatrix(renderer->display_list++, G_MTX_MODELVIEW);
    gDPPipeSync(renderer->display_list++);
}

void renderer_entity_start(Renderer* renderer, Entity* entity){
    renderer_set_transform(renderer, &entity->transform, &entity->dl_matrix);
}

void renderer_entity_end(Renderer* renderer) {
    renderer_pop_transform(renderer);
}

void renderer_get_screen_size(Renderer* renderer, IVec2* screen_size) {
    *screen_size = renderer->screen_size;
}

void renderer_set_fill_color(Renderer* renderer, Color* color) {
    renderer->fill_color = GPACK_RGBA5551(color->r, color->g, color->b, 255);

    if (renderer->display_list) {
        gDPSetFillColor(renderer->display_list++, (renderer->fill_color << 16 | renderer->fill_color));
    }
}

void renderer_set_fill_mode(Renderer* renderer) {
    gDPSetFillColor(renderer->display_list++, (renderer->fill_color << 16 | renderer->fill_color));
    gDPSetCycleType(renderer->display_list++, G_CYC_FILL);
    //TODO: do i need to sync pipe here?
}

void renderer_draw_filled_rect(Renderer* renderer, IRect* rect) {
    gDPFillRectangle(renderer->display_list++, rect->x, rect->y, rect->x + rect->width, rect->y + rect->height);
    gDPPipeSync(renderer->display_list++);
}

static void _draw_sprite_slice(Renderer* renderer, ImageSprite* sprite, int frame, int x, int y) {
    int slice_width = image_sprite_get_slice_width(sprite);
    int slice_height = image_sprite_get_slice_height(sprite);

    uint32_t frame_offset = (slice_width * slice_height * 2) * frame;

    gDPLoadTextureBlock(renderer->display_list++, sprite->data + frame_offset, G_IM_FMT_RGBA, G_IM_SIZ_16b, slice_width, slice_height, 0, 
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

void renderer_draw_sprite_slice(Renderer* renderer, ImageSprite* sprite, int frame, int x, int y) {
    renderer_set_shading_mode(renderer, SHADING_MODE_SPRITE);
    _draw_sprite_slice(renderer, sprite, frame, x, y);
}

void renderer_draw_sprite(Renderer* renderer, ImageSprite* sprite, int x, int y) {
    renderer_set_shading_mode(renderer, SHADING_MODE_SPRITE);

    int slice_width = image_sprite_get_slice_width(sprite);
    int slice_height = image_sprite_get_slice_height(sprite);
    int slice = 0;

    for (uint8_t row = 0; row < sprite->vslices; row++ ) {
        int draw_y = y + row * slice_height;
        for (uint8_t col = 0; col < sprite->hslices; col++) {
            int draw_x = x + slice_width * col;

            _draw_sprite_slice(renderer, sprite, slice++, draw_x, draw_y);
        }
    }
}

void renderer_draw_text(Renderer* renderer, Font* font, int x, int y, char* text) {
    if (!text || text[0] == 0) return;
    renderer_set_shading_mode(renderer, SHADING_MODE_SPRITE);
/*
    TODO: Color blending? need better alpha than 5551
    gDPSetPrimColor(renderer->display_list++, 255, 255, color->r, color->g, color->b, 255);
    gDPSetCombineMode(renderer->display_list++, G_CC_MODULATERGBA_PRIM , G_CC_MODULATERGBA_PRIM );
*/
    
    char ch = text[0];
    uint16_t glyph_index = font_get_glyph_index(font, ch);
    FontGlyph* glyph = font->glyphs + glyph_index;
    uint16_t stride = font->spritefont_tile_width * font->spritefont_tile_height * 2;

    int caret = x + glyph->left;

    while (ch) {
        glyph_index = font_get_glyph_index(font, ch);
        glyph = font->glyphs + glyph_index;

        int draw_pos_x = caret + glyph->left;
        int draw_pos_y = y + glyph->top;

        gDPLoadTextureBlock(renderer->display_list++, font->spritefont + (stride * glyph_index), 
        G_IM_FMT_RGBA, G_IM_SIZ_16b, 
        font->spritefont_tile_width, font->spritefont_tile_height, 0, 
        G_TX_CLAMP, G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

        gDPLoadSync(renderer->display_list++);

        gSPTextureRectangle(renderer->display_list++, 
            draw_pos_x << 2, draw_pos_y << 2, 
            (draw_pos_x + font->spritefont_tile_width) << 2, (draw_pos_y + font->spritefont_tile_height) << 2,
            G_TX_RENDERTILE, 
            0 << 5, 0 << 5, 
            1 << 10, 1 << 10);

        gDPPipeSync(renderer->display_list++);

        caret += glyph->advance;
        text++;
        ch = text[0];
    }
}

void render_billboard_frame(Renderer* renderer, BillboardQuad* quad, int frame, int index) {
    int slice_width = image_sprite_get_slice_width(quad->sprite);
    int slice_height = image_sprite_get_slice_height(quad->sprite);

    uint32_t frame_offset = (slice_width * slice_height * 2) * frame;

    gDPLoadTextureBlock(renderer->display_list++, 
    quad->sprite->data + frame_offset,  G_IM_FMT_RGBA, G_IM_SIZ_16b,  
    slice_width, slice_height, 
    0, G_TX_WRAP, G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

    gSP2Triangles(renderer->display_list++, 
        index + 0, index + 1, index + 2, 0, 
        index + 0, index + 2, index + 3, 0);
}

void renderer_draw_billboard_quad(Renderer* renderer, BillboardQuad* quad) {
    billboard_quad_look_at_camera(quad, renderer->camera);

    renderer_set_transform(renderer, &quad->transform, &quad->dl_matrix);

    gSPClearGeometryMode(renderer->display_list++, G_LIGHTING);
    gDPSetRenderMode(renderer->display_list++, G_RM_AA_ZB_TEX_EDGE, G_RM_AA_ZB_TEX_EDGE2);
    gSPTexture(renderer->display_list++, 0x8000, 0x8000, 0, 0, G_ON );
    gDPSetCombineMode(renderer->display_list++, G_CC_DECALRGBA, G_CC_DECALRGBA);
    gDPSetTexturePersp(renderer->display_list++, G_TP_PERSP);

        switch (quad->type) {
        case BILLBOARD_QUAD_1X1:
            gSPVertex(renderer->display_list++, quad->vertices, 4, 0);
            render_billboard_frame(renderer, quad, quad->frame, 0);
            break;
        
        case BILLBOARD_QUAD_2X2:
            gSPVertex(renderer->display_list++, quad->vertices, 16, 0);
            render_billboard_frame(renderer, quad, 0, 0);
            render_billboard_frame(renderer, quad, 1, 4);
            render_billboard_frame(renderer, quad, 2, 8);
            render_billboard_frame(renderer, quad, 3, 12);
            break;
    }


    renderer_pop_transform(renderer);
}

void renderer_draw_static_mesh(Renderer* renderer, Mesh* mesh) {
    for (uint32_t i = 0 ; i < mesh->info.primitive_count; i++) {
        Primitive* primitive = mesh->primitives + i;
        
        renderer_set_shading_mode(renderer, primitive->material.mode);

        if (primitive->material.mode == SHADING_MODE_GOURAUD_TEXTURED) {
            ImageSprite* texture = mesh->textures + primitive->material.texture;

            gDPLoadTextureBlock(renderer->display_list++, texture->data,
            G_IM_FMT_RGBA, G_IM_SIZ_16b,  texture->width, texture->height, 0,
            G_TX_WRAP, G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        }
        
        gSPSetLights1(renderer->display_list++, mesh->colors[primitive->material.color]);
        gSPDisplayList(renderer->display_list++, mesh->display_list + primitive->display_list);
        gDPPipeSync(renderer->display_list++);
    }
}