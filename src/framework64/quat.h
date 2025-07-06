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

/** Returns a vec3 containing euler angles as (x = pitch, y = yaw, z = roll) 
 *  Note: the angles in the returned Vec3 are expressed in radians.
*/
Vec3 quat_to_euler(const Quat* q);

/** Sets the specified quat from the provided euler angles.
 *  Note: the angles passed into this function should be specified in degrees.
 */
void quat_from_euler(Quat* q, float x, float y, float z);
void quat_slerp(const Quat* a, const Quat* b, float t, Quat* out);

#ifdef __cplusplus
}
#endif
