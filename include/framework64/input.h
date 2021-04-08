#ifndef FW64_INPUT_H
#define FW64_INPUT_H

#include "vec2.h"

#include <nusys.h>

typedef enum {
    CONTROLLER_BUTTON_A = A_BUTTON,
    CONTROLLER_BUTTON_B = B_BUTTON,
    CONTROLLER_BUTTON_L = L_TRIG,
    CONTROLLER_BUTTON_R = R_TRIG,
    CONTROLLER_BUTTON_Z = Z_TRIG,
    CONTROLLER_BUTTON_START = START_BUTTON,
    CONTROLLER_BUTTON_C_UP = U_CBUTTONS,
    CONTROLLER_BUTTON_C_DOWN = D_CBUTTONS,
    CONTROLLER_BUTTON_C_LEFT = L_CBUTTONS,
    CONTROLLER_BUTTON_C_RIGHT = R_CBUTTONS,
    CONTROLLER_BUTTON_DPAD_UP = U_JPAD,
    CONTROLLER_BUTTON_DPAD_DOWN = D_JPAD,
    CONTROLLER_BUTTON_DPAD_LEFT = L_JPAD,
    CONTROLLER_BUTTON_DPAD_RIGHT = R_JPAD
} ControllerButton;

typedef struct {
    NUContData current_state[NU_CONT_MAXCONTROLLERS];
    NUContData previous_state[NU_CONT_MAXCONTROLLERS];
} Input;

void input_init(Input* input);
void input_update(Input* input);

int input_button_pressed(Input* input, int controller, ControllerButton button);
int input_button_down(Input* input, int controller, ControllerButton button);
void input_stick(Input* input, int controller, Vec2* current);

#endif