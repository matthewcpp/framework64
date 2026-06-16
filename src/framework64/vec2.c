#include "framework64/vec2.h"

#include "framework64/types.h"
#include "framework64/math.h"

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

void vec2_normalize(Vec2* v) {
    float len = v->x * v->x + v->y * v->y;

    if (len > 0) {
        len = 1.0f / fw64_sqrtf(len);
    }

    v->x *= len;
    v->y *= len;
}

float vec2_length(const Vec2* v) {
    return fw64_sqrtf(v->x * v->x + v->y * v->y);
}

float vec2_length_squared(const Vec2* v) {
    return v->x * v->x + v->y * v->y;
}

void ivec2_set(IVec2* vec, int x, int y) {
    vec->x = x;
    vec->y = y;
}

void ivec2_add(const IVec2* a, const IVec2* b, IVec2* out) {
    out->x = a->x + b->x;
    out->y = a->y + b->y;
}
