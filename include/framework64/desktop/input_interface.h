#pragma once

#include "framework64/vec2.h"

#include <SDL2/SDL.h>

#include <array>
#include <cstdint>

namespace framework64 {
struct Controller {
    std::array<uint8_t, SDL_CONTROLLER_BUTTON_MAX> buttons;
    std::array<float, SDL_CONTROLLER_AXIS_MAX> axis;
};

class InputInterface {
public:
public:
    virtual bool buttonPressed(Controller const & current, Controller const & previous, int button) = 0;
    virtual bool buttonDown(Controller const & current, int button) = 0;
    virtual Vec2 stick(Controller const & current, int stick_index) = 0;

    virtual void updateControllerFromKeyboard(Controller & controller, uint8_t const * sdl_keyboard_state) = 0;
};
}