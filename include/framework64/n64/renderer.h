#ifndef FW64_N64_RENDERER_H
#define FW64_N64_RENDERER_H

#include <nusys.h>

/* The maximum length of the display list of one task  */
#define GFX_DLIST_LEN     2048

struct fw64Renderer{
    // holds the current command insertion point of the display list
    Gfx* display_list;
    Gfx* display_list_start;

    // display list for drawing commands
    Gfx gfx_list[GFX_DLIST_LEN];

    fw64Camera* camera;

    u16 fill_color;
    u16 clear_color;

    Vp view_port;
    IVec2 screen_size;
    fw64RenderMode render_mode;
    fw64ShadingMode shading_mode;
};

#endif