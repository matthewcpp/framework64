#include "framework64/input.h"
#include "framework64/desktop/input.h"

#include "framework64/n64/controller_button.h"

void fw64Input::init(framework64::InputInterface& interface_) {
    interface = &interface_;
    interface->setInput(&input_state);

    int joystick_count = SDL_NumJoysticks();
    int controller_index = 0;

    for (int i = 0; i < joystick_count; i++) {
        if (SDL_IsGameController(i)) {
            input_state.controllers[controller_index++] = SDL_GameControllerOpen(i);

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
    SDL_GameController* controller = input_state.controllers[controller_index];
    auto & controller_state = input_state.current_states[controller_index];

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
    input_state.previous_states = input_state.current_states;

    for (int i = 0; i < 4; i++) {
        if(input_state.controllers[i]) {
            updateControllerState(i);
        }
    }
}

void fw64Input::onDeviceAdded(int index) {
    for (int i = 0; i < 4; i++) {
        if (input_state.controllers[i] == nullptr) {
            input_state.controllers[i] = SDL_GameControllerOpen(index);
        }
    }
}


// C interface

int fw64_input_button_pressed(fw64Input* input, int controller, int button) {
    return input->interface->buttonPressed(controller, button);
}

int fw64_input_button_down(fw64Input* input, int controller, int button) {
    return input->interface->buttonDown(controller, button);
}

void fw64_input_stick(fw64Input* input, int controller, Vec2* current) {
    *current = input->interface->stick(controller, 0);
}