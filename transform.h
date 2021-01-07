#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "quat.h"
#include "vec3.h"

typedef struct {
    Vec3 position;
    Quat rotation;
    Vec3 scale;
} Transform;

void transform_init(Transform* transform);
void transform_forward(Transform* transform, Vec3* out);
void transform_up(Transform* transform, Vec3* out);

#endif