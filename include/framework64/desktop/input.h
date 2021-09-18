#pragma once

#include "framework64/input.h"
#include "framework64/desktop/input_interface.h"

#include <memory>


struct fw64Input {
public:
    void init(framework64::InputInterface& interface_);
    void update();
    void onDeviceAdded(int index);

    framework64::InputInterface* interface = nullptr;

private:
    void updateControllerState(int controller_index);

    framework64::InputState input_state;
};