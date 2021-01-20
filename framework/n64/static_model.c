#include "ultra/static_model.h"

#include "n64_logo.h"
#include "suzanne.h"
#include "penguin.h"

#include <string.h>

void static_model_bounding_box(int handle, Box* box) {
    switch (handle) {
        case 1:
            memcpy(box, &n64_logo_bounding_box[0], sizeof(Box));
        break;

        case 2:
            memcpy(box, &suzanne_bounding_box[0], sizeof(Box));
        break;

        case 3:
            memcpy(box, &penguin_bounding_box[0], sizeof(Box));
        break;

        case 4:
            memcpy(box, &penguin_bounding_box[0], sizeof(Box));
        break;
    }
}

void static_model_render(int handle, Renderer* renderer) {
    switch (handle) {
        case 1:
            gSPDisplayList(renderer->display_list++, n64_logo_mesh_0_dl);
            gSPDisplayList(renderer->display_list++, n64_logo_mesh_1_dl);
            gSPDisplayList(renderer->display_list++, n64_logo_mesh_2_dl);
            gSPDisplayList(renderer->display_list++, n64_logo_mesh_3_dl);
            break;

        case 2:
            gSPDisplayList(renderer->display_list++, suzanne_mesh_0_dl);
            break;

        case 3:
            gSPClearGeometryMode(renderer->display_list++, G_LIGHTING); // do i want to render with lighting?
            gDPSetCycleType(renderer->display_list++,G_CYC_1CYCLE);
            gDPSetRenderMode(renderer->display_list++, G_RM_AA_ZB_TEX_EDGE, G_RM_AA_ZB_TEX_EDGE2);
            gSPTexture(renderer->display_list++, 0x8000, 0x8000, 0, 0, G_ON );
            gDPSetCombineMode(renderer->display_list++,G_CC_DECALRGBA  , G_CC_DECALRGBA  );
            //gDPSetCombineMode(renderer->display_list++,G_CC_MODULATERGBA , G_CC_MODULATERGBA );
            gDPSetTexturePersp(renderer->display_list++, G_TP_PERSP);
            gDPSetTextureFilter(renderer->display_list++,G_TF_POINT);

            gDPLoadTextureBlock( renderer->display_list++, model_penguin_image_0_image,
                G_IM_FMT_RGBA, G_IM_SIZ_16b,  32, 32, 0,
                G_TX_WRAP, G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

            gSPDisplayList(renderer->display_list++, penguin_mesh_0_dl);
            gDPPipeSync(renderer->display_list++);

                        gDPLoadTextureBlock( renderer->display_list++, model_penguin_image_1_image,
                G_IM_FMT_RGBA, G_IM_SIZ_16b,  32, 32, 0,
                G_TX_WRAP, G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

            gSPDisplayList(renderer->display_list++, penguin_mesh_1_dl);
            break;

        case 4:
            gDPSetCycleType(renderer->display_list++,G_CYC_1CYCLE);
            gDPSetRenderMode(renderer->display_list++, G_RM_AA_ZB_TEX_EDGE, G_RM_AA_ZB_TEX_EDGE2);
            gSPTexture(renderer->display_list++, 0x8000, 0x8000, 0, 0, G_ON );
            gDPSetCombineMode(renderer->display_list++,G_CC_MODULATERGBA , G_CC_MODULATERGBA );
            gDPSetTexturePersp(renderer->display_list++, G_TP_PERSP);
            gDPSetTextureFilter(renderer->display_list++,G_TF_POINT);

            gDPLoadTextureBlock( renderer->display_list++, model_penguin_image_0_image,
                G_IM_FMT_RGBA, G_IM_SIZ_16b,  32, 32, 0,
                G_TX_WRAP, G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

            gSPDisplayList(renderer->display_list++, penguin_mesh_0_dl);
            gDPPipeSync(renderer->display_list++);

                        gDPLoadTextureBlock( renderer->display_list++, model_penguin_image_1_image,
                G_IM_FMT_RGBA, G_IM_SIZ_16b,  32, 32, 0,
                G_TX_WRAP, G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

            gSPDisplayList(renderer->display_list++, penguin_mesh_1_dl);
            break;
    }
}