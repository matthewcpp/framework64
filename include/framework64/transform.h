#ifndef FW64_TRANSFORM_H
#define FW64_TRANSFORM_H

#include "framework64/quat.h"
#include "framework64/vec3.h"

#include <nusys.h>

typedef struct {
    Vec3 position;
    Quat rotation;
    Vec3 scale;
    Mtx matrix;
} Transform;

void transform_init(Transform* transform);
void transform_forward(Transform* transform, Vec3* out);
void transform_back(Transform* transform, Vec3* out);
void transform_up(Transform* transform, Vec3* out);
void transform_down(Transform* transform, Vec3* out);
void transform_left(Transform* transform, Vec3* out);
void transform_right(Transform* transform, Vec3* out);

void transform_look_at(Transform* transform, Vec3* target, Vec3* up);
void transform_update_matrix(Transform* transform);
#endif