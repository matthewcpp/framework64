#include "static_model.h"

#include "assets/build_n64/n64_logo.h"
#include "assets/build_n64/suzanne.h"

#include <string.h>

void static_model_bounding_box(int handle, Box* box) {
    switch (handle) {
        case 1:
            memcpy(box, &n64_logo_bounding_box[0], sizeof(Box));
        break;

        case 2:
            memcpy(box, &suzanne_bounding_box[0], sizeof(Box));
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
    }
    
    return 0;
}