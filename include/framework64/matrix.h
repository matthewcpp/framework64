#ifndef FW64_MATRIX_H
#define FW64_MATRIX_H

#include "framework64/vec2.h"
#include "framework64/vec3.h"
#include "framework64/quat.h"

void matrix_set_identity(float* matrix);
void matrix_from_trs(float* out, Vec3* t, Quat* r, Vec3* s);
void matrix_target_to(float* out, Vec3* eye, Vec3* target, Vec3* up);
void matrix_get_scaling(float* mat, Vec3* out);
void matrix_get_rotation(float* mat, Quat* out);
void matrix_multiply(float* out, float* a, float* b);

void matrix_perspective(float* out, float fov_degrees, float aspect, float near, float far);
void matrix_camera_look_at(float* out, Vec3* eye, Vec3* target, Vec3* up);

/** Creates a rotation matrix for the supplied angle in radians. */
void mat2_set_rotation(float* mat, float rad);

/** Transforms the vector according to the matrix */
void mat2_transform_vec2(const float* mat, Vec2* vec);

#endif