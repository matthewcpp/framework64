#include "framework64/input.h"

#include <nusys.h>

#include <string.h>
#include <malloc.h>

void input_init(Input* input) {
    nuContInit(); // initialize nusys controller subsystem
}

void input_update(Input* input) {
    memcpy(&input->previous_state[0], &input->current_state[0], sizeof(NUContData) * NU_CONT_MAXCONTROLLERS);

    nuContDataGetExAll(input->current_state);
}

int input_button_pressed(Input* input, int controller, ControllerButton button) {
    return input->current_state[controller].trigger & button;
}

int input_button_down(Input* input, int controller, ControllerButton button) {
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

static float map_controller_axis_value(float value, float min_val, float max_val) {
    if (value < min_val)
        value = min_val;
    else if (value > max_val)
        value = max_val;

    float t = ((float)value - min_val) /  (max_val - min_val);
    float result =  AXIS_MAPPED_MIN + t * (AXIS_MAPPED_MAX - AXIS_MAPPED_MIN);
    return result;
}

void input_stick(Input* input, int controller, Vec2* current) {
    current->x = map_controller_axis_value((float)input->current_state[controller].stick_x, CONTROLLER_STICK_MIN_X, CONTROLLER_STICK_MAX_X);
    current->y = map_controller_axis_value((float)input->current_state[controller].stick_y, CONTROLLER_STICK_MIN_Y, CONTROLLER_STICK_MAX_Y);
}