#pragma once

#include "framework64/desktop/input_interface.h"

namespace framework64 {

class N64InputInterface : public InputInterface {
public:
    virtual void setInput(InputState const * input_state) override;
    virtual bool buttonPressed(int controller_index, int button) override;
    virtual bool buttonDown(int controller_index, int button) override;
    virtual Vec2 stick(int controller_index, int stick_index) override;

private:
    InputState const * input;
};

}