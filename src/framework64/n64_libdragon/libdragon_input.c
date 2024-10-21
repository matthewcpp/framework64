#include "libdragon_input.h"

#include <string.h>

void fw64_n64_libdragon_input_update(fw64Input* input) {
    memcpy(input->previous_input, input->current_input, sizeof(input->previous_input));

    for (int port = 0; port < 4; port++) {
        input->current_input[port] = joypad_get_inputs(port);
    }
}

int fw64_input_controller_button_pressed(fw64Input* input, int controller, int button) {
    return !(input->previous_input[controller].btn.raw & button) && (input->current_input[controller].btn.raw & button);
}

int fw64_input_controller_button_released(fw64Input* input, int controller, int button) {
    return (input->previous_input[controller].btn.raw & button) && !(input->current_input[controller].btn.raw & button);
}

int fw64_input_controller_button_down(fw64Input* input, int controller, int button) {
    return (input->current_input[controller].btn.raw & button);
}

#define AXIS_MAPPED_MIN -1.0f
#define AXIS_MAPPED_MAX 1.0f

//refer to joypad.h
#define CONTROLLER_STICK_MIN_X -61.0f
#define CONTROLLER_STICK_MAX_X 61.0f

#define CONTROLLER_STICK_MIN_Y -63.0f
#define CONTROLLER_STICK_MAX_Y 63.0f

static float fw64_map_controller_axis_value(float value, float min_val, float max_val) {
    if (value < min_val)
        value = min_val;
    else if (value > max_val)
        value = max_val;

    float t = ((float)value - min_val) /  (max_val - min_val);
    float result =  AXIS_MAPPED_MIN + t * (AXIS_MAPPED_MAX - AXIS_MAPPED_MIN);
    return result;
}

void fw64_input_controller_stick(fw64Input* input, int controller, Vec2* current) {
    current->x = fw64_map_controller_axis_value(input->current_input[controller].stick_x, CONTROLLER_STICK_MIN_X, CONTROLLER_STICK_MAX_X);
    current->y = fw64_map_controller_axis_value(input->current_input[controller].stick_y, CONTROLLER_STICK_MIN_Y, CONTROLLER_STICK_MAX_Y);
}