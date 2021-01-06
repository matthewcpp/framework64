#ifndef RENDERER_H
#define RENDERER_H

#include <nusys.h>

#include "dynamic.h"
#include "camera.h"

typedef struct {
    // holds the current command insertion point of the display list
    Gfx* display_list;

    // display list for drawing commands
    Gfx gfx_list[GFX_GLIST_LEN];
} Renderer;


Renderer* renderer_create();
void renderer_begin(Renderer* renderer, Camera* camera);
void renderer_end(Renderer* renderer);

void renderer_draw_static(Renderer* renderer, Dynamic* dynamic, const Gfx* static_display_list);

#endif