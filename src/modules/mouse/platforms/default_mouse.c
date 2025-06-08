#include "fw64_mouse_module.h"

#include <stddef.h>

fw64MouseModule* fw64_mouse_module_init() {
    return NULL;
}

void fw64_mouse_module_update(fw64MouseModule* module) {

}

int fw64_mouse_button_is_pressed(fw64MouseModule* module, fw64MouseButton button) {
    return 0;
}

int fw64_mouse_button_is_released(fw64MouseModule* module, fw64MouseButton button) {
    return 0;
}

int fw64_mouse_button_is_down(fw64MouseModule* module, fw64MouseButton button) {
    return 0;
}

IVec2 fw64_mouse_get_position(fw64MouseModule* module) {
    IVec2 pos = ivec2_zero();

    return pos;
}