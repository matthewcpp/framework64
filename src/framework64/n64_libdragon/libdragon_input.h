#pragma once


#include "framework64/input.h"

#include <libdragon.h>

struct fw64Input {
    joypad_inputs_t previous_input[4];
    joypad_inputs_t current_input[4];
};

void fw64_n64_libdragon_input_update(fw64Input* input);
