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