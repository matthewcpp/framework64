#include "framework64/vec2.h"

void vec2_set(Vec2* v, float x, float y) {
    v->x = x;
    v->y = y;
}

void vec2_add(const Vec2* a, const Vec2* b, Vec2* out) {
    out->x = a->x + b->x;
    out->y = a->y + b->y;
}

void vec2_scale(const Vec2* v, float k, Vec2* out) {
    out->x = v->x * k;
    out->y = v->y * k;
}

void ivec2_set(IVec2* vec, int x, int y) {
    vec->x = x;
    vec->y = y;
}

void ivec2_add(const IVec2* a, const IVec2* b, IVec2* out) {
    out->x = a->x + b->x;
    out->y = a->y + b->y;
}
