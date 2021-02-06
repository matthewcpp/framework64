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
void transform_back(Transform* transform, Vec3* out);
void transform_up(Transform* transform, Vec3* out);
void transform_down(Transform* transform, Vec3* out);
void transform_left(Transform* transform, Vec3* out);
void transform_right(Transform* transform, Vec3* out);

void transform_look_at(Transform* transform, Vec3* target, Vec3* up);
#endif