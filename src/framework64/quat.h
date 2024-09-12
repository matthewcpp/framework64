#pragma once

/** \file quat.h */

#include "framework64/vec3.h"

typedef struct {
    float x, y, z, w;
} Quat;

#ifdef __cplusplus
extern "C" {
#endif

void quat_ident(Quat* q);
void quat_set(Quat* q, float x, float y, float z, float w);
void quat_set_axis_angle(Quat* q, float x, float y, float z, float rad);
void quat_transform_vec3(Vec3* out, Quat* q, Vec3* a);
void quat_normalize(Quat* q);

/** Equivalent to the inverse of the quaternion.  
  * Ensure that the quaternion is normalized before calling this method.
  */
void quat_conjugate(Quat* q);

Vec3 quat_to_euler(Quat* q);

void quat_from_euler(Quat* q, float x, float y, float z);
void quat_slerp(Quat* out, Quat* a, Quat* b, float t);

#ifdef __cplusplus
}
#endif
