#include "framework64/input.h"
#include "framework64/desktop/input.h"

#include "framework64/n64/controller_button.h"

#include <iostream>

void fw64Input::init(framework64::InputInterface& interface_, fw64Time& time_) {
    interface = &interface_;
    time = &time_;

    int joystick_count = SDL_NumJoysticks();
    int controller_index = 0;

    std::fill(controller_rumble_durations.begin(), controller_rumble_durations.end(), 0.0f);

    for (int i = 0; i < joystick_count; i++) {
        if (SDL_IsGameController(i)) {
            sdl_gamecontrollers[controller_index++] = SDL_GameControllerOpen(i);

            if (controller_index == 4)
                break;
        }
    }
}

#define AXIS_MIN_VAL -32768.0f
#define AXIS_MAX_VAL 32767.0f
#define AXIS_MAPPED_MIN -1.0f
#define AXIS_MAPPED_MAX 1.0f

#define TRIGGER_MIN_VAL 0.0f
#define TRIGGER_MAX_VAL 32767.0f
#define TRIGGER_MAPPED_MIN 0.0f
#define TRIGGER_MAPPED_MAX 1.0f

static float map_value_range(float value, float initial_min, float initial_max, float mapped_min, float mapped_max) {
    float t = ((float)value - initial_min) /  (initial_max - initial_min);
    return mapped_min + t * (mapped_max - mapped_min);
}

#define MAP_CONTROLLER_AXIS_VALUE(val) \
    map_value_range(static_cast<float>((val)), AXIS_MIN_VAL, AXIS_MAX_VAL, AXIS_MAPPED_MIN, AXIS_MAPPED_MAX)

#define MAP_CONTROLLER_TRIGGER_VALUE(val) \
    map_value_range(static_cast<float>((val)), TRIGGER_MIN_VAL, TRIGGER_MAX_VAL, TRIGGER_MAPPED_MIN, TRIGGER_MAPPED_MAX)

void fw64Input::updateControllerState(int controller_index) {
    SDL_GameController* controller = sdl_gamecontrollers[controller_index];
    auto & controller_state = current_controller_states[controller_index];

    for (int b = (int)SDL_CONTROLLER_BUTTON_INVALID + 1 ; b < SDL_CONTROLLER_BUTTON_MAX; b++) {
        controller_state.buttons[b] = SDL_GameControllerGetButton(controller, static_cast<SDL_GameControllerButton>(b));
    }

    // Note that in SDL pressing "up" on the analog stick will give you negative values.
    controller_state.axis[SDL_CONTROLLER_AXIS_RIGHTX] = MAP_CONTROLLER_AXIS_VALUE(SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTX));
    controller_state.axis[SDL_CONTROLLER_AXIS_RIGHTY] = -MAP_CONTROLLER_AXIS_VALUE(SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTY));

    controller_state.axis[SDL_CONTROLLER_AXIS_LEFTX] = MAP_CONTROLLER_AXIS_VALUE(SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX));
    controller_state.axis[SDL_CONTROLLER_AXIS_LEFTY] = -MAP_CONTROLLER_AXIS_VALUE(SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY));

    controller_state.axis[SDL_CONTROLLER_AXIS_TRIGGERLEFT] = MAP_CONTROLLER_TRIGGER_VALUE(SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_TRIGGERLEFT));
    controller_state.axis[SDL_CONTROLLER_AXIS_TRIGGERRIGHT] = MAP_CONTROLLER_TRIGGER_VALUE(SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_TRIGGERRIGHT));
}

void fw64Input::update() {
    previous_controller_states = current_controller_states;

    for (int i = 0; i < 4; i++) {
        if(sdl_gamecontrollers[i]) {
            updateControllerState(i);
        }
        else if (i == 0) {
            interface->updateControllerFromKeyboard(current_controller_states[i], SDL_GetKeyboardState(nullptr));
        }

        if (controller_rumble_durations[i] > 0.0f) {
            controller_rumble_durations[i] -= time->time_delta;
        }
    }
}

void fw64Input::onDeviceAdded(int index) {
    std::cout << "Controller added to port: " << index << std::endl;

    if (index < 4) {
        sdl_gamecontrollers[index] = SDL_GameControllerOpen(index);
    }
}

void fw64Input::onDeviceRemoved(int index) {
    std::cout << "Controller removed from port: " << index << std::endl;

    if (index < 4) {
        SDL_GameControllerClose(sdl_gamecontrollers[index]);
        sdl_gamecontrollers[index] = nullptr;
        controller_rumble_durations[index] = 0.0f;
    }
}

bool fw64Input::controllerIsConnected(int index) const {
    // for now we always will have a keyboard controller in port 0
    if (index == 0)
        return true;

    return sdl_gamecontrollers[index] != nullptr;
}

bool fw64Input::buttonPressed(int controller, int button) {
    if (controllerIsConnected(controller))
        return interface->buttonPressed(current_controller_states[controller], previous_controller_states[controller], button);
    else
        return false;
}
bool fw64Input::buttonReleased(int controller, int button) {
    if (controllerIsConnected(controller))
        return interface->buttonReleased(current_controller_states[controller], previous_controller_states[controller], button);
    else
        return false;
}
bool fw64Input::buttonDown(int controller, int button) {
    if (controllerIsConnected(controller))
        return interface->buttonDown(current_controller_states[controller], button);
    else
        return false;
}
Vec2 fw64Input::stick(int controller, int stick_index) {
    if (controllerIsConnected(controller))
        return interface->stick(current_controller_states[controller], stick_index);
    else
        return {0.0f, 0.0f};
}

bool fw64Input::controllerHasRumble(int index) const {
    return SDL_JoystickHasRumble(SDL_GameControllerGetJoystick(sdl_gamecontrollers[index]));
}

int fw64Input::controllerSetRumble(int index, float value, float duration) {
    auto rumble_val = static_cast<uint16_t>(value * std::numeric_limits<uint16_t>::max());
    auto ms = static_cast<uint32_t>(duration * 1000);

    controller_rumble_durations[index] = value > 0.0f ? duration : 0.0f;

    return SDL_JoystickRumble(SDL_GameControllerGetJoystick(sdl_gamecontrollers[index]), rumble_val, rumble_val, ms);
}

bool fw64Input::controllerIsRumbling(int index) const {
    return controller_rumble_durations[index] > 0.0f;
}

// C interface

int fw64_input_controller_is_connected(fw64Input* input, int controller) {
    return input->controllerIsConnected(controller) ? 1 : 0;
}

int fw64_input_controller_button_pressed(fw64Input* input, int controller, int button) {
    return input->buttonPressed(controller, button);
}

int fw64_input_controller_button_released(fw64Input* input, int controller, int button) {
    return input->buttonReleased(controller, button);
}

int fw64_input_controller_button_down(fw64Input* input, int controller, int button) {
    return input->buttonDown(controller, button);
}

void fw64_input_controller_stick(fw64Input* input, int controller, Vec2* current) {
    *current = input->stick(controller, 0);
}

int fw64_input_controller_has_rumble(fw64Input* input, int controller) {
    return input->controllerHasRumble(controller);
}

int fw64_input_controller_set_rumble(fw64Input* input, int controller, float value, float duration) {
    return input->controllerSetRumble(controller, value, duration);
}

int fw64_input_controller_rumble_active(fw64Input* input, int controller) {
    return input->controllerIsRumbling(controller);
}