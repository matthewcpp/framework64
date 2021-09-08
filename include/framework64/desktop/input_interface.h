#pragma once

#include "framework64/vec2.h"

#include <SDL2/SDL.h>

#include <array>
#include <cstdint>

namespace framework64 {
struct InputState {
public:
    struct Controller {
        std::array<uint8_t, SDL_CONTROLLER_BUTTON_MAX> buttons;
        std::array<float, SDL_CONTROLLER_AXIS_MAX> axis;
    };

    std::array<SDL_GameController*, 4> controllers = {nullptr, nullptr, nullptr, nullptr};
    std::array<Controller, 4> current_states;
    std::array<Controller, 4> previous_states;

    [[nodiscard]] inline bool controllerIsConnected(int controller_index) const { return controllers[controller_index] != nullptr; }
};

class InputInterface {
public:
public:
    virtual void setInput(InputState const * input_state) = 0;
    virtual bool buttonPressed(int controller_index, int button) = 0;
    virtual bool buttonDown(int controller_index, int button) = 0;
    virtual Vec2 stick(int controller_index, int stick_index) = 0;
};
}