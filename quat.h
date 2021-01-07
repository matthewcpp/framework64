#ifndef QUAT_H
#define QUAT_H

#include "vec3.h"

typedef struct {
    float x, y, z, w;
} Quat;

void quat_ident(Quat* q);
void quat_set_axis_angle(Quat* q, float x, float y, float z, float rad);
void quat_transform_vec3(Vec3* out, Quat* q, Vec3* a);
void quat_normalize(Quat* q);
#endif
