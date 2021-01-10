#include "static_model.h"

#include "assets/build/cube.h"

#include <string.h>

void static_model_bounding_box(int handle, Box* box) {
    memcpy(box, &cube_bounding_box[0], sizeof(Box));

    (void)handle;
}

int static_model_render(int handle, Gfx* display_list) {
    gSPDisplayList(display_list, cube_mesh_0_dl);

    (void)handle;
    return 1;
}