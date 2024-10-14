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
    //return (input->current_input[controller].btn.raw & button);
}

int fw64_input_controller_button_released(fw64Input* input, int controller, int button) {

}

int fw64_input_controller_button_down(fw64Input* input, int controller, int button) {

}