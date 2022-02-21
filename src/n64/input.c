#include "framework64/n64/input.h"

#include <nusys.h>

#include <string.h>
#include <malloc.h>

void fw64_n64_input_init(fw64Input* input, fw64Time* time) {
    nuContInit(); // initialize nusys controller subsystem

    memset(input, 0, sizeof(fw64Input));
    input->time = time;

    // check for rumble packs
    for (int i = 0; i < NU_CONT_MAXCONTROLLERS; i++) {
        int result = nuContRmbCheck(i); // 0 error code means rumble pack initialized

        if (result == 0) {
            nuContRmbModeSet(i, NU_CONT_RMB_MODE_ENABLE);
            input->rumble_enabled[i] = 1;
        }
    }
}

void fw64_n64_input_update(fw64Input* input) {
    memcpy(&input->previous_state[0], &input->current_state[0], sizeof(NUContData) * NU_CONT_MAXCONTROLLERS);

    nuContDataGetExAll(input->current_state);

    for (int i = 0; i < NU_CONT_MAXCONTROLLERS; i++) {
        if (input->rumble_time[i] > 0.0f) {
            input->rumble_time[i] -= input->time->time_delta;

            if (input->rumble_time[i] <= 0.0f) {
                nuContRmbStop(i);
            }
        }
    }
}

int fw64_input_controller_button_pressed(fw64Input* input, int controller, int button) {
    return input->current_state[controller].trigger & button;
}

int fw64_input_controller_button_released(fw64Input* input, int controller, int button) {
    return (!(input->current_state[controller].button & button)) && (input->previous_state[controller].button & button);
}

int fw64_input_controller_button_down(fw64Input* input, int controller, int button) {
    return input->current_state[controller].button & button;
}

#define AXIS_MAPPED_MIN -1.0f
#define AXIS_MAPPED_MAX 1.0f

// Ther assumption here is that these values are the actual mappable range.
// In reality the controllers can return values outside of this range
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
    current->x = fw64_map_controller_axis_value((float)input->current_state[controller].stick_x, CONTROLLER_STICK_MIN_X, CONTROLLER_STICK_MAX_X);
    current->y = fw64_map_controller_axis_value((float)input->current_state[controller].stick_y, CONTROLLER_STICK_MIN_Y, CONTROLLER_STICK_MAX_Y);
}

int fw64_input_controller_is_connected(fw64Input* input, int controller) {
    return input->current_state[controller].errno == 0;
}

int fw64_input_controller_has_rumble(fw64Input* input, int controller) {
    return input->rumble_enabled[controller];
}

int fw64_input_controller_set_rumble(fw64Input* input, int controller, float value, float duration) {
    if (value == 0.0f && input->rumble_time[controller] > 0.0f) {
        nuContRmbStop(controller);
        input->rumble_time[controller] = 0.0f;
    }
    else {
        u16 frequency = (u16)(value * 256.0f);
        nuContRmbStart(controller, frequency, 10000);
        input->rumble_time[controller] = duration;
    }
}

int fw64_input_controller_rumble_active(fw64Input* input, int controller) {
    return input->rumble_time[controller] > 0.0f;
}
