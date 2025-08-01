#include "framework64/vec3.h"

#include "framework64/types.h"
#include "framework64/math.h"

#include <math.h>
#include <stdlib.h>

// Implementation based on: https://glmatrix.net/docs/vec3.js.html

void vec3_set(Vec3* v, float x, float y, float z) {
    v->x = x;
    v->y = y;
    v->z = z;
}

void vec3_set_all(Vec3* v, float val) {
    v->x = val;
    v->y = val;
    v->z = val;
}

void vec3_copy(const Vec3* src, Vec3* dest) {
    dest->x = src->x;
    dest->y = src->y;
    dest->z = src->z;
}

void vec3_add(const Vec3* a, const Vec3* b, Vec3* out) {
    out->x = a->x + b->x;
    out->y = a->y + b->y;
    out->z = a->z + b->z;
}

void vec3_add_and_scale(const Vec3* a, const Vec3* b, float k, Vec3* out) {
    out->x = a->x + b->x * k;
    out->y = a->y + b->y * k;
    out->z = a->z + b->z * k;
}

void vec3_subtract(const Vec3* a, const Vec3* b, Vec3* out) {
    out->x = a->x - b->x;
    out->y = a->y - b->y;
    out->z = a->z - b->z;
}

void vec3_scale(const Vec3* v, float k, Vec3* out) {
    out->x = v->x * k;
    out->y = v->y * k;
    out->z = v->z * k;
}

float vec3_distance(const Vec3* a, const Vec3* b) {
    float x = b->x - a->x;
    float y = b->y - a->y;
    float z = b->z - a->z;

    return fw64_sqrtf(x * x + y * y + z * z);
}

float vec3_distance_squared(const Vec3* a, const Vec3* b) {
    float x = b->x - a->x;
    float y = b->y - a->y;
    float z = b->z - a->z;

    return x * x + y * y + z * z;
}

void vec3_normalize(Vec3* v) {
    float len = v->x * v->x + v->y * v->y + v->z * v->z;

    if (len > 0) {
        len = 1.0f / fw64_sqrtf(len);
    }

    v->x *= len;
    v->y *= len;
    v->z *= len;
}

void vec3_negate(Vec3* v) {
    v->x *= -1.0f;
    v->y *= -1.0f;
    v->z *= -1.0f;
}

void vec3_cross(const Vec3* a, const Vec3* b, Vec3* out) {
    out->x = a->y * b->z - a->z * b->y;
    out->y = a->z * b->x - a->x * b->z;
    out->z = a->x * b->y - a->y * b->x;
}

float vec3_dot(const Vec3* a, const Vec3* b) {
    return a->x * b->x + a->y * b->y + a->z * b->z;
}

float vec3_length(const Vec3* v) {
    return fw64_sqrtf(v->x * v->x + v->y * v->y + v->z * v->z);
}

float vec3_length_squared(const Vec3* v) {
    return v->x * v->x + v->y * v->y + v->z * v->z;
}

int vec3_equals(const Vec3* a, const Vec3* b, float epsilon) {
    float dx = fabsf(a->x - b->x);
    float dy = fabsf(a->y - b->y);
    float dz = fabsf(a->z - b->z);

    return dx <= epsilon * fmaxf(1.0f, fmaxf(fabsf(a->x), fabsf(b->x))) &&
            dy <= epsilon * fmaxf(1.0f, fmaxf(fabsf(a->y), fabsf(b->y))) &&
            dz <= epsilon * fmaxf(1.0f, fmaxf(fabsf(a->z), fabsf(b->z)));
}

void vec3_smoothstep(const Vec3* a, const Vec3* b, float t, Vec3* out) {
    vec3_lerp(a, b, fw64_smoothstep(0.0f, 1.0f, t), out);
}

void vec3_lerp(const Vec3* a, const Vec3* b, float t, Vec3* out) {
    float one_minus_t = 1.0f - t;
    out->x = a->x * one_minus_t + b->x * t;
    out->y = a->y * one_minus_t + b->y * t;
    out->z = a->z * one_minus_t + b->z * t;
}
