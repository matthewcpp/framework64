#pragma once

#include "framework64/input.h"
#include "framework64/desktop/input_interface.h"

#include "framework64/time.h"

#include <memory>

struct fw64Input {
public:
    void init(framework64::InputInterface& interface_, fw64Time& time_);
    void update();
    void onDeviceAdded(int index);
    void onDeviceRemoved(int index);

    bool buttonPressed(int controller, int button);
    bool buttonReleased(int controller, int button);
    bool buttonDown(int controller, int button);
    Vec2 stick(int controller, int stick_index);

    framework64::InputInterface* interface = nullptr;

    [[nodiscard]] bool controllerIsConnected(int index) const;
    [[nodiscard]] bool controllerHasRumble(int index) const;
    [[nodiscard]] bool controllerIsRumbling(int index) const;
    int controllerSetRumble(int index, float value, float duration);

private:

private:
    void updateControllerState(int controller_index);

    fw64Time* time;

    std::array<framework64::Controller, 4> current_controller_states;
    std::array<framework64::Controller, 4> previous_controller_states;
    std::array<float, 4> controller_rumble_durations;
    std::array<SDL_GameController*, 4> sdl_gamecontrollers = {nullptr, nullptr, nullptr, nullptr};
};