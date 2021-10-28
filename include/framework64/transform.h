#pragma once

/** \file transform.h */

#include "framework64/quat.h"
#include "framework64/types.h"
#include "framework64/vec3.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    Vec3 position;
    Quat rotation;
    Vec3 scale;
    fw64Matrix matrix;
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

void fw64_transform_mult_point(fw64Transform* transform, Vec3* point, Vec3* out);
void fw64_transform_inv_mult_point(fw64Transform* transform, Vec3* point, Vec3* out);

#ifdef __cplusplus
}
#endif

