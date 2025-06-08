#pragma once

#include "fw64_mouse_module.h"

#include <SDL2/SDL.h>

struct fw64MouseModule{
    Uint32 prev_state, current_state;
    IVec2 prev_pos, current_pos;
};
