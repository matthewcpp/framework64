#include "static_model.h"

#include "assets/build/cube.h"
#include "assets/build/n64_logo.h"

#include <string.h>

void static_model_bounding_box(int handle, Box* box) {
    switch (handle) {
        case 0:
            memcpy(box, &cube_bounding_box[0], sizeof(Box));
        break;

        case 1:
            memcpy(box, &n64_logo_bounding_box[0], sizeof(Box));
        break;
    }
}

int static_model_render(int handle, Gfx* display_list) {
    switch (handle) {
        case 0:
            gSPDisplayList(display_list, cube_mesh_0_dl);
            return 1;
        break;

        case 1:
            gSPDisplayList(display_list++, n64_logo_mesh_0_dl);
            gSPDisplayList(display_list++, n64_logo_mesh_1_dl);
            gSPDisplayList(display_list++, n64_logo_mesh_2_dl);
            gSPDisplayList(display_list++, n64_logo_mesh_3_dl);
            return 4;
    }
    
    return 0;
}