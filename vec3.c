#include "vec3.h"

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

void vec3_add(Vec3* out, Vec3* a, Vec3* b) {
    out->x = a->x + b->x;
    out->y = a->y + b->y;
    out->z = a->z + b->z;
}
