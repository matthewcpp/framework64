#include "framework64/desktop/n64_input_interface.h"

#include "framework64/n64/controller_button.h"

#define TRIGGER_THRESHOLD 0.25f
#define AXIS_THRESHOLD 0.4f

namespace framework64 {

static bool button_down(framework64::InputState::Controller const & state, int button) {
    switch (button) {
        case FW64_N64_CONTROLLER_BUTTON_A:
            return state.buttons[SDL_CONTROLLER_BUTTON_A];

        case FW64_N64_CONTROLLER_BUTTON_B:
            return state.buttons[SDL_CONTROLLER_BUTTON_X];

        case FW64_N64_CONTROLLER_BUTTON_L:
            return state.buttons[SDL_CONTROLLER_BUTTON_LEFTSHOULDER];

        case FW64_N64_CONTROLLER_BUTTON_R:
            return state.buttons[SDL_CONTROLLER_BUTTON_RIGHTSHOULDER];

        case FW64_N64_CONTROLLER_BUTTON_Z:
            return state.axis[SDL_CONTROLLER_AXIS_TRIGGERLEFT] >= TRIGGER_THRESHOLD;

        case FW64_N64_CONTROLLER_BUTTON_START:
            return state.buttons[SDL_CONTROLLER_BUTTON_START];

        case FW64_N64_CONTROLLER_BUTTON_C_UP:
            return state.axis[SDL_CONTROLLER_AXIS_RIGHTY] >= AXIS_THRESHOLD;

        case FW64_N64_CONTROLLER_BUTTON_C_DOWN:
            return state.axis[SDL_CONTROLLER_AXIS_RIGHTY] <= -AXIS_THRESHOLD;

        case FW64_N64_CONTROLLER_BUTTON_C_LEFT:
            return state.axis[SDL_CONTROLLER_AXIS_RIGHTX] <= -AXIS_THRESHOLD;

        case FW64_N64_CONTROLLER_BUTTON_C_RIGHT:
            return state.axis[SDL_CONTROLLER_AXIS_RIGHTX] >= AXIS_THRESHOLD;

        case FW64_N64_CONTROLLER_BUTTON_DPAD_UP:
            return state.buttons[SDL_CONTROLLER_BUTTON_DPAD_UP];

        case FW64_N64_CONTROLLER_BUTTON_DPAD_DOWN:
            return state.buttons[SDL_CONTROLLER_BUTTON_DPAD_DOWN];

        case FW64_N64_CONTROLLER_BUTTON_DPAD_LEFT:
            return state.buttons[SDL_CONTROLLER_BUTTON_DPAD_LEFT];

        case FW64_N64_CONTROLLER_BUTTON_DPAD_RIGHT:
            return state.buttons[SDL_CONTROLLER_BUTTON_DPAD_RIGHT];
        default:
            return false;
    }
}

void N64InputInterface::setInput(InputState const * input_state) {
    input = input_state;
}

bool N64InputInterface::buttonPressed(int controller_index, int button) {
    if (!input->controllerIsConnected(controller_index)) return false;

    return !button_down(input->previous_states[controller_index], button) && button_down(input->current_states[controller_index], button);
}

bool N64InputInterface::buttonDown(int controller_index, int button) {
    if (!input->controllerIsConnected(controller_index)) return false;

    return button_down(input->current_states[controller_index], button);
}

Vec2 N64InputInterface::stick(int controller_index, int stick_index) {
    Vec2 result = {0.0f, 0.0f};

    if (input->controllerIsConnected(controller_index)) {
        auto const & controller = input->current_states[controller_index];

        result.x = controller.axis[SDL_CONTROLLER_AXIS_LEFTX];
        result.y = controller.axis[SDL_CONTROLLER_AXIS_LEFTY];
    }

    return result;
}



}