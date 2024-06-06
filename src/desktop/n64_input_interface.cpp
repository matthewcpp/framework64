#include "framework64/desktop/n64_input_interface.hpp"

#include "framework64/controller_mapping/n64.h"

#define TRIGGER_THRESHOLD 0.25f
#define AXIS_THRESHOLD 0.4f

namespace framework64 {

bool N64InputInterface::buttonPressed(Controller const & current, Controller const & previous, int button) {
    return buttonIsDown(current, button) && !buttonIsDown(previous, button);
}

bool N64InputInterface::buttonReleased(Controller const & current, Controller const & previous, int button) {
    return !buttonIsDown(current, button) && buttonIsDown(previous, button);
}

bool N64InputInterface::buttonDown(Controller const & current, int button) {
    return buttonIsDown(current, button);
}

Vec2 N64InputInterface::stick(Controller const & current, int) {
    Vec2 result = {0.0f, 0.0f};

    result.x = current.axis[SDL_CONTROLLER_AXIS_LEFTX];
    result.y = current.axis[SDL_CONTROLLER_AXIS_LEFTY];

    return result;
}

void N64InputInterface::updateControllerFromKeyboard(Controller & controller, uint8_t const * sdl_keyboard_state) {
    controller.buttons[SDL_CONTROLLER_BUTTON_A] = sdl_keyboard_state[SDL_SCANCODE_X];
    controller.buttons[SDL_CONTROLLER_BUTTON_X] = sdl_keyboard_state[SDL_SCANCODE_C];
    controller.axis[SDL_CONTROLLER_AXIS_TRIGGERLEFT] = sdl_keyboard_state[SDL_SCANCODE_Z] ? 1.0f : 0.0f;
    controller.buttons[SDL_CONTROLLER_BUTTON_START] = sdl_keyboard_state[SDL_SCANCODE_RETURN];
    controller.buttons[SDL_CONTROLLER_BUTTON_LEFTSHOULDER] = sdl_keyboard_state[SDL_SCANCODE_A];
    controller.buttons[SDL_CONTROLLER_BUTTON_RIGHTSHOULDER] = sdl_keyboard_state[SDL_SCANCODE_S];
    controller.buttons[SDL_CONTROLLER_BUTTON_DPAD_UP] = sdl_keyboard_state[SDL_SCANCODE_I];
    controller.buttons[SDL_CONTROLLER_BUTTON_DPAD_DOWN] = sdl_keyboard_state[SDL_SCANCODE_K];
    controller.buttons[SDL_CONTROLLER_BUTTON_DPAD_RIGHT] = sdl_keyboard_state[SDL_SCANCODE_L];
    controller.buttons[SDL_CONTROLLER_BUTTON_DPAD_LEFT] = sdl_keyboard_state[SDL_SCANCODE_J];

    float stick_modifier = (sdl_keyboard_state[SDL_SCANCODE_LSHIFT] || sdl_keyboard_state[SDL_SCANCODE_RSHIFT]) ? 0.5f : 1.0f;

    if (sdl_keyboard_state[SDL_SCANCODE_RIGHT]) {
        controller.axis[SDL_CONTROLLER_AXIS_LEFTX] = 1.0f * stick_modifier;
    }
    else if (sdl_keyboard_state[SDL_SCANCODE_LEFT]) {
        controller.axis[SDL_CONTROLLER_AXIS_LEFTX] = -1.0f * stick_modifier;
    }
    else {
        controller.axis[SDL_CONTROLLER_AXIS_LEFTX] = 0;
    }

    if (sdl_keyboard_state[SDL_SCANCODE_UP]) {
        controller.axis[SDL_CONTROLLER_AXIS_LEFTY] = 1.0f * stick_modifier;
    }
    else if (sdl_keyboard_state[SDL_SCANCODE_DOWN]) {
        controller.axis[SDL_CONTROLLER_AXIS_LEFTY] = -1.0f * stick_modifier;
    }
    else {
        controller.axis[SDL_CONTROLLER_AXIS_LEFTY] = 0;
    }

    if (sdl_keyboard_state[SDL_SCANCODE_T]) {
        controller.axis[SDL_CONTROLLER_AXIS_RIGHTY] = 1.0f;
    }
    else if (sdl_keyboard_state[SDL_SCANCODE_G]) {
        controller.axis[SDL_CONTROLLER_AXIS_RIGHTY] = -1.0f;
    }
    else {
        controller.axis[SDL_CONTROLLER_AXIS_RIGHTY] = 0.0f;
    }

    if (sdl_keyboard_state[SDL_SCANCODE_H]) {
        controller.axis[SDL_CONTROLLER_AXIS_RIGHTX] = 1.0f;
    }
    else if (sdl_keyboard_state[SDL_SCANCODE_F]) {
        controller.axis[SDL_CONTROLLER_AXIS_RIGHTX] = -1.0f;
    }
    else {
        controller.axis[SDL_CONTROLLER_AXIS_RIGHTX] = 0.0f;
    }
}

bool N64InputInterface::buttonIsDown(Controller const & state, int button) {
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

}