#include "framework64/vec3.h"

#include <math.h>

void vec3_zero(Vec3* v) {
    v->x = 0.0f;
    v->y = 0.0f;
    v->z = 0.0f;
}

void vec3_one(Vec3* v) {
    v->x = 1.0f;
    v->y = 1.0f;
    v->z = 1.0f;
}

void vec3_set(Vec3* v, float x, float y, float z) {
    v->x = x;
    v->y = y;
    v->z = z;
}

void vec3_add(Vec3* out, Vec3* a, Vec3* b) {
    out->x = a->x + b->x;
    out->y = a->y + b->y;
    out->z = a->z + b->z;
}

void vec3_subtract(Vec3* out, Vec3* a, Vec3* b) {
    out->x = a->x - b->x;
    out->y = a->y - b->y;
    out->z = a->z - b->z;
}

void vec3_scale(Vec3* out, Vec3* v, float k) {
    out->x = v->x * k;
    out->y = v->y * k;
    out->z = v->z * k;
}

float vec3_distance(Vec3* a, Vec3* b) {
    float x = b->x - a->x;
    float y = b->y - a->y;
    float z = b->z - a->z;

    return _nsqrtf(x * x + y * y + z * z);
}

void vec3_normalize(Vec3* v) {
    float len = v->x * v->x + v->y * v->y + v->z * v->z;

    if (len > 0) {
        len = 1.0f / _nsqrtf(len);
    }

    v->x *= len;
    v->y *= len;
    v->z *= len;
}
