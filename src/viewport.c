#include "framework64/viewport.h"

#include "framework64/display.h"

#ifdef FW64_PLATFORM_N64_LIBULTRA
void fw64_n64_libultra_update_viewport(fw64Viewport* viewport) {
    viewport->_n64_viewport.vp.vscale[0] = viewport->size.x * 2;
    viewport->_n64_viewport.vp.vscale[1] = viewport->size.y * 2;
    viewport->_n64_viewport.vp.vscale[2] = G_MAXZ / 2;
    viewport->_n64_viewport.vp.vscale[3] = 0;

    viewport->_n64_viewport.vp.vtrans[0] = ((viewport->position.x * 2) + viewport->size.x) * 2;
    viewport->_n64_viewport.vp.vtrans[1] = ((viewport->position.y * 2) + viewport->size.y) * 2;
    viewport->_n64_viewport.vp.vtrans[2] = G_MAXZ / 2;
    viewport->_n64_viewport.vp.vtrans[3] = 0;
}
#endif

void fw64_viewport_set(fw64Viewport* viewport, IVec2* position, IVec2* size) {
    viewport->position = *position;
    viewport->size = *size;

#ifdef FW64_PLATFORM_N64_LIBULTRA
    fw64_n64_libultra_update_viewport(viewport);
#endif
}

void fw64_viewport_set_relative_to_display(fw64Viewport* viewport, fw64Display* display, Vec2* position, Vec2* size) {
    Vec2 screen_size = fw64_display_get_size_f(display);

    viewport->position.x = (int)(screen_size.x * position->x);
    viewport->position.y = (int)(screen_size.y * position->y);
    viewport->size.x = (int)(screen_size.x * size->x);
    viewport->size.y = (int)(screen_size.y * size->y);

#ifdef FW64_PLATFORM_N64_LIBULTRA
    fw64_n64_libultra_update_viewport(viewport);
#endif
}
