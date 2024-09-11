#include "framework64/vec4.h"

void vec4_set(Vec4* v, float x, float y, float z, float w) {
    v->x = x;
    v->y = y;
    v->z = z;
    v->w = w;
}

void vec4_div(Vec4* v, float d, Vec4* out) {
    out->x = v->x / d;
    out->y = v->y / d;
    out->z = v->z / d;
    out->w = v->w / d;
}

void vec4_negate(Vec4* v) {
    v->x *= -1.0f;
    v->y *= -1.0f;
    v->z *= -1.0f;
    v->w *= -1.0f;
}
