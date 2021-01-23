#include "ultra/vec2.h"

void ivec2_add(IVec2* out, IVec2* a, IVec2* b) {
    out->x = a->x + b->x;
    out->y = a->y + b->y;
}