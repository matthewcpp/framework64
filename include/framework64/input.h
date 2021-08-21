#ifndef FW64_INPUT_H
#define FW64_INPUT_H

#include "vec2.h"

typedef struct fw64Input fw64Input;

void fw64_input_update(fw64Input* input);

int fw64_input_button_pressed(fw64Input* input, int controller, int button);
int fw64_input_button_down(fw64Input* input, int controller, int button);
void fw64_input_stick(fw64Input* input, int controller, Vec2* current);

#endif