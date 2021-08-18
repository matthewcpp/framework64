#ifndef FW64_N64_INPUT_H
#define FW64_N64_INPUT_H

#include <nusys.h>

struct fw64Input{
    NUContData current_state[NU_CONT_MAXCONTROLLERS];
    NUContData previous_state[NU_CONT_MAXCONTROLLERS];
};

#endif