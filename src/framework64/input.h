#pragma once

/** \file input.h */

#include "vec2.h"

#define FW64_MAX_CONTROLLER_COUNT 4

typedef struct fw64Input fw64Input;

#ifdef __cplusplus
extern "C" {
#endif

int fw64_input_controller_is_connected(fw64Input* input, int controller);
int fw64_input_controller_button_pressed(fw64Input* input, int controller, int button);
int fw64_input_controller_button_released(fw64Input* input, int controller, int button);
int fw64_input_controller_button_down(fw64Input* input, int controller, int button);
void fw64_input_controller_stick(fw64Input* input, int controller, Vec2* current);

int fw64_input_controller_has_rumble(fw64Input* input, int controller);
int fw64_input_controller_rumble_active(fw64Input* input, int controller);
int fw64_input_controller_set_rumble(fw64Input* input, int controller, float value, float duration);

#ifdef __cplusplus
}
#endif
