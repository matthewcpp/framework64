#include "static_model.h"

#include "assets/build_n64/n64_logo.h"

#include <string.h>

void static_model_bounding_box(int handle, Box* box) {
    switch (handle) {
        case 1:
            memcpy(box, &n64_logo_bounding_box[0], sizeof(Box));
        break;
    }
}

int static_model_render(int handle, Gfx* display_list) {
    switch (handle) {

        case 1:
            gSPDisplayList(display_list++, n64_logo_mesh_0_dl);
            gSPDisplayList(display_list++, n64_logo_mesh_1_dl);
            gSPDisplayList(display_list++, n64_logo_mesh_2_dl);
            gSPDisplayList(display_list++, n64_logo_mesh_3_dl);
            return 4;
    }
    
    return 0;
}