#ifndef INPUT_H
#define INPUT_H

#include <nusys.h>

typedef struct {
    NUContData 	current_state[NU_CONT_MAXCONTROLLERS];
    NUContData 	previous_state[NU_CONT_MAXCONTROLLERS];
} Input;

Input* input_create();
void input_update(Input* input);

int input_button_pressed(Input* input, int controller, int button);
int input_button_down(Input* input, int controller, int button);

#endif