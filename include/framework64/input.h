#ifndef FW64_INPUT_H
#define FW64_INPUT_H

#include "vec2.h"

#include <nusys.h>

typedef enum {
    FW64_CONTROLLER_BUTTON_A = A_BUTTON,
    FW64_CONTROLLER_BUTTON_B = B_BUTTON,
    FW64_CONTROLLER_BUTTON_L = L_TRIG,
    FW64_CONTROLLER_BUTTON_R = R_TRIG,
    FW64_CONTROLLER_BUTTON_Z = Z_TRIG,
    FW64_CONTROLLER_BUTTON_START = START_BUTTON,
    FW64_CONTROLLER_BUTTON_C_UP = U_CBUTTONS,
    FW64_CONTROLLER_BUTTON_C_DOWN = D_CBUTTONS,
    FW64_CONTROLLER_BUTTON_C_LEFT = L_CBUTTONS,
    FW64_CONTROLLER_BUTTON_C_RIGHT = R_CBUTTONS,
    FW64_CONTROLLER_BUTTON_DPAD_UP = U_JPAD,
    FW64_CONTROLLER_BUTTON_DPAD_DOWN = D_JPAD,
    FW64_CONTROLLER_BUTTON_DPAD_LEFT = L_JPAD,
    FW64_CONTROLLER_BUTTON_DPAD_RIGHT = R_JPAD
} fw64ControllerButton;

typedef struct {
    NUContData current_state[NU_CONT_MAXCONTROLLERS];
    NUContData previous_state[NU_CONT_MAXCONTROLLERS];
} fw64Input;

void fw64_input_init(fw64Input* input);
void fw64_input_update(fw64Input* input);

int fw64_input_button_pressed(fw64Input* input, int controller, fw64ControllerButton button);
int fw64_input_button_down(fw64Input* input, int controller, fw64ControllerButton button);
void fw64_input_stick(fw64Input* input, int controller, Vec2* current);

#endif