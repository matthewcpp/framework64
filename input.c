#include "input.h"

#include <string.h>
#include <malloc.h>

Input* input_create() {
    Input* input = calloc(1, sizeof(Input));
    return input;
}

void input_update(Input* input) {
    memcpy(&input->previous_state[0], &input->current_state[0], sizeof(NUContData) * NU_CONT_MAXCONTROLLERS);

    nuContDataGetExAll(input->current_state);
}

int input_button_pressed(Input* input, int controller, int button) {
    return input->current_state[controller].trigger & button;
}

int input_button_down(Input* input, int controller, int button) {
    return input->current_state[controller].button & button;
}