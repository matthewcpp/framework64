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
void quat_transform_vec3(const Quat* q, const Vec3* a, Vec3* out);
void quat_normalize(Quat* q);

/** Equivalent to the inverse of the quaternion.  
  * Ensure that the quaternion is normalized before calling this method.
  */
void quat_conjugate(Quat* q);

Vec3 quat_to_euler(const Quat* q);

void quat_from_euler(Quat* q, float x, float y, float z);
void quat_slerp(const Quat* a, const Quat* b, float t, Quat* out);

#ifdef __cplusplus
}
#endif
