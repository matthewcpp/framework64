#include "framework64/vec2.h"

void vec2_set(Vec2* v, float x, float y) {
    v->x = x;
    v->y = y;
}

void vec2_add(Vec2* out, Vec2* a, Vec2* b) {
    out->x = a->x + b->x;
    out->y = a->y + b->y;
}

void vec2_scale(Vec2* out, Vec2* v, float k) {
    out->x = v->x * k;
    out->y = v->y * k;
}

void ivec2_set(IVec2* vec, int x, int y) {
    vec->x = x;
    vec->y = y;
}

void ivec2_add(IVec2* out, IVec2* a, IVec2* b) {
    out->x = a->x + b->x;
    out->y = a->y + b->y;
}