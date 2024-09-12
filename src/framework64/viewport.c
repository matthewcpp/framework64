#include "framework64/viewport.h"

#include "framework64/display.h"

void fw64_viewport_set(fw64Viewport* viewport, const IVec2* position, const IVec2* size) {
    viewport->position = *position;
    viewport->size = *size;
}

void fw64_viewport_set_relative_to_display(fw64Viewport* viewport, fw64Display* display, Vec2* position, Vec2* size) {
    Vec2 screen_size = fw64_display_get_size_f(display);

    viewport->position.x = (int)(screen_size.x * position->x);
    viewport->position.y = (int)(screen_size.y * position->y);
    viewport->size.x = (int)(screen_size.x * size->x);
    viewport->size.y = (int)(screen_size.y * size->y);
}
