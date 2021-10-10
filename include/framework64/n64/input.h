#pragma once

#include <nusys.h>

struct fw64Input{
    NUContData current_state[NU_CONT_MAXCONTROLLERS];
    NUContData previous_state[NU_CONT_MAXCONTROLLERS];
};

void fw64_n64_input_init(fw64Input* input);
void fw64_n64_input_update(fw64Input* input);