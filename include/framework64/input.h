#pragma once

/** \file input.h */

#include "vec2.h"

typedef struct fw64Input fw64Input;

#ifdef __cplusplus
extern "C" {
#endif

int fw64_input_button_pressed(fw64Input* input, int controller, int button);
int fw64_input_button_released(fw64Input* input, int controller, int button);
int fw64_input_button_down(fw64Input* input, int controller, int button);
void fw64_input_stick(fw64Input* input, int controller, Vec2* current);

#ifdef __cplusplus
}
#endif
