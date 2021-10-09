#pragma once

#include "framework64/desktop/input_interface.h"

namespace framework64 {

class N64InputInterface : public InputInterface {
public:
    virtual bool buttonPressed(Controller const & current, Controller const & previous, int button) override;
    virtual bool buttonDown(Controller const & current, int button) override;
    virtual Vec2 stick(Controller const & current, int stick_index) override;

    virtual void updateControllerFromKeyboard(Controller & controller, uint8_t const * sdl_keyboard_state) override;

private:
    static bool buttonIsDown(Controller const & state, int button);
};

}