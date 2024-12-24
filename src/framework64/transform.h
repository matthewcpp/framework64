#pragma once

/** \file transform.h */

#include "framework64/box.h"
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
    float world_matrix[16];
} fw64Transform;

void fw64_transform_init(fw64Transform* transform);
void fw64_transform_forward(const fw64Transform* transform, Vec3* out);
void fw64_transform_back(const fw64Transform* transform, Vec3* out);
void fw64_transform_up(const fw64Transform* transform, Vec3* out);
void fw64_transform_down(const fw64Transform* transform, Vec3* out);
void fw64_transform_left(const fw64Transform* transform, Vec3* out);
void fw64_transform_right(const fw64Transform* transform, Vec3* out);

void fw64_transform_look_at(fw64Transform* transform, const Vec3* target,const  Vec3* up);
void fw64_transform_update_matrix(fw64Transform* transform);
void fw64_transform_xform_box(const fw64Transform* transform, const Box* source, Box* target);

void fw64_transform_mult_point(const fw64Transform* transform, const Vec3* point, Vec3* out);
void fw64_transform_inv_mult_point(const fw64Transform* transform, const Vec3* point, Vec3* out);

#ifdef __cplusplus
}
#endif

