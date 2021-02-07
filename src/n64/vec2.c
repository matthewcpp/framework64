#include "framework64/vec2.h"

void vec2_set(Vec2* v, float x, float y) {
    v->x = x;
    v->y = y;
}

void ivec2_add(IVec2* out, IVec2* a, IVec2* b) {
    out->x = a->x + b->x;
    out->y = a->y + b->y;
}