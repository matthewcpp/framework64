#pragma once

#include <framework64/vec2.h>

typedef struct fw64MouseModule fw64MouseModule;

typedef enum {
    FW64_MOUSE_BUTTON_LEFT = 1,
    FW64_MOUSE_BUTTON_MIDDLE = 2,
    FW64_MOUSE_BUTTON_RIGHT = 3
} fw64MouseButton;

#ifdef __cplusplus
extern "C" {
#endif

fw64MouseModule* fw64_mouse_module_init();

void fw64_mouse_module_update(fw64MouseModule* module);

int fw64_mouse_button_is_pressed(fw64MouseModule* module, fw64MouseButton button);
int fw64_mouse_button_is_released(fw64MouseModule* module, fw64MouseButton button);
int fw64_mouse_button_is_down(fw64MouseModule* module, fw64MouseButton button);

IVec2 fw64_mouse_get_position(fw64MouseModule* module);


#ifdef __cplusplus
}
#endif