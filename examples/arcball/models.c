#include "models.h"

#include "framework64/box.h"

#include "penguin_model.h"
#include "suzanne_model.h"
#include "n64_logo_model.h"

#include "buttons_sprite.h"

#include <string.h>

void n64logo_init(N64Logo* n64logo) {
    entity_init(&n64logo->entity);
    memcpy(&n64logo->entity.bounding, &n64_logo_bounding_box[0], sizeof(Box));
}

void n64logo_draw(N64Logo* n64logo, Renderer* renderer) {
    renderer_entity_start(renderer, &n64logo->entity);
    gSPDisplayList(renderer->display_list++, n64_logo_mesh_0_dl);
    gSPDisplayList(renderer->display_list++, n64_logo_mesh_1_dl);
    gSPDisplayList(renderer->display_list++, n64_logo_mesh_2_dl);
    gSPDisplayList(renderer->display_list++, n64_logo_mesh_3_dl);
    renderer_entity_end(renderer);
}

void suzanne_init(Suzanne* suzanne){
    entity_init(&suzanne->entity);
    memcpy(&suzanne->entity.bounding, &suzanne_bounding_box[0], sizeof(Box));
}

void suzanne_draw(Suzanne* suzanne, Renderer* renderer){
    gSPSetGeometryMode(renderer->display_list++, G_LIGHTING)
    gDPSetCombineMode(renderer->display_list++, G_CC_SHADE, G_CC_SHADE);

    renderer_entity_start(renderer, &suzanne->entity);
    gSPDisplayList(renderer->display_list++, suzanne_mesh_0_dl);
    renderer_entity_end(renderer);
}

void penguin_init(Penguin* penguin) {
    entity_init(&penguin->entity);
    penguin->draw_with_lighting = 0;
    memcpy(&penguin->entity.bounding, &penguin_bounding_box[0], sizeof(Box));
}

void penguin_draw(Penguin* penguin, Renderer* renderer) {
    renderer_entity_start(renderer, &penguin->entity);

    if (!penguin->draw_with_lighting) {
        gSPClearGeometryMode(renderer->display_list++, G_LIGHTING);
        gDPSetCombineMode(renderer->display_list++,G_CC_DECALRGBA  , G_CC_DECALRGBA  );
    }
    else {
        gSPSetGeometryMode(renderer->display_list++, G_LIGHTING)
        gDPSetCombineMode(renderer->display_list++, G_CC_SHADE, G_CC_SHADE);
        gDPSetCombineMode(renderer->display_list++,G_CC_MODULATERGBA , G_CC_MODULATERGBA );
    }

    gDPSetCycleType(renderer->display_list++,G_CYC_1CYCLE);
    gDPSetRenderMode(renderer->display_list++, G_RM_AA_ZB_TEX_EDGE, G_RM_AA_ZB_TEX_EDGE2);
    gSPTexture(renderer->display_list++, 0x8000, 0x8000, 0, 0, G_ON );
    gDPSetTexturePersp(renderer->display_list++, G_TP_PERSP);
    gDPSetTextureFilter(renderer->display_list++,G_TF_POINT);
    
    gDPLoadTextureBlock(renderer->display_list++, model_penguin_image_0,
    G_IM_FMT_RGBA, G_IM_SIZ_16b,  32, 32, 0,
    G_TX_WRAP, G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

    gSPDisplayList(renderer->display_list++, penguin_mesh_0_dl);
    gDPPipeSync(renderer->display_list++);

    gDPLoadTextureBlock( renderer->display_list++, model_penguin_image_1,
    G_IM_FMT_RGBA, G_IM_SIZ_16b,  32, 32, 0,
    G_TX_WRAP, G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

    gSPDisplayList(renderer->display_list++, penguin_mesh_1_dl);

    renderer_entity_end(renderer);
}

void init_buttons_sprite(ImageSprite* buttons_sprite) {
    buttons_sprite->width = buttons_SPRITE_WIDTH;
    buttons_sprite->height = buttons_SPRITE_HEIGHT;
    buttons_sprite->hslices = buttons_SPRITE_HSLICES;
    buttons_sprite->vslices = buttons_SPRITE_VSLICES;
    buttons_sprite->slice_count = buttons_SPRITE_SLICE_COUNT;
    buttons_sprite->slices = &buttons_sprite_slices[0];
}
