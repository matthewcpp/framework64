#include "framework64/input.h"
#include "framework64/desktop/input.h"

void fw64_input_update(fw64Input* input) {

}

int fw64_input_button_pressed(fw64Input* input, int controller, int button) {
    return 0;
}

int fw64_input_button_down(fw64Input* input, int controller, int button) {
    return 0;
}

void fw64_input_stick(fw64Input* input, int controller, Vec2* current) {
    current->x = 0.0f;
    current->y = 0.0f;
}