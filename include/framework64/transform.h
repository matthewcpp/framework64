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
} fw64Transform;

void fw64_transform_init(fw64Transform* transform);
void fw64_transform_forward(fw64Transform* transform, Vec3* out);
void fw64_transform_back(fw64Transform* transform, Vec3* out);
void fw64_transform_up(fw64Transform* transform, Vec3* out);
void fw64_transform_down(fw64Transform* transform, Vec3* out);
void fw64_transform_left(fw64Transform* transform, Vec3* out);
void fw64_transform_right(fw64Transform* transform, Vec3* out);

void fw64_transform_look_at(fw64Transform* transform, Vec3* target, Vec3* up);
void fw64_transform_update_matrix(fw64Transform* transform);
#endif