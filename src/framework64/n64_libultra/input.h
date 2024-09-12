#pragma once

#include "framework64/input.h"
#include "framework64/time.h"

#include <nusys.h>

struct fw64Input{
    NUContData current_state[NU_CONT_MAXCONTROLLERS];
    NUContData previous_state[NU_CONT_MAXCONTROLLERS];
    fw64Time* time;
    int rumble_enabled[NU_CONT_MAXCONTROLLERS];
    float rumble_time[NU_CONT_MAXCONTROLLERS];
};

void fw64_n64_input_init(fw64Input* input, fw64Time* time);
void fw64_n64_input_update(fw64Input* input);
