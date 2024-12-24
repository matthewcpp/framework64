#pragma once

/** \file matrix.h */

#include "framework64/vec2.h"
#include "framework64/vec3.h"
#include "framework64/vec4.h"
#include "framework64/quat.h"

#ifdef __cplusplus
extern "C" {
#endif

// 4x4 matrix

void matrix_set_identity(float* matrix);
void matrix_translate(float* matrix, Vec3* t);
void matrix_from_trs(float* matrix, const Vec3* t, const Quat* r, const Vec3* s);
void matrix_from_quat(float* matrix, const Quat* q);
void matrix_target_to(float* matrix, const Vec3* eye, const Vec3* target, const Vec3* up);
void matrix_get_scaling(const float* mat, Vec3* out);
void matrix_get_rotation(const float* mat, Quat* out);
void matrix_multiply(const float* a, const float* b, float* out);
void matrix_transpose(float* matrix);
int matrix_invert(const float* matrix, float* out);
void matrix_transform_vec3(const float* matrix, Vec3* vec);
void matrix_transform_vec4(const float* matrix, Vec4* vec);

void matrix_perspective(float* out, float fov_degrees, float aspect, float near, float far);
void matrix_ortho(float* out, float left, float right, float bottom, float top, float near, float far);
void matrix_ortho2d(float* out, float left, float right, float bottom, float top);
void matrix_camera_look_at(float* out, Vec3* eye, Vec3* target, Vec3* up);

// 3x3 matrix

void mat3_set_from_mat4(float* mat3, const float* mat4);
void mat3_transform_vec3(const float* mat3, Vec3* vec);

// 2x2 matrix

/** Creates a rotation matrix for the supplied angle in radians. */
void mat2_set_rotation(float* mat, float rad);

/** Transforms the vector according to the matrix */
void mat2_transform_vec2(const float* mat, Vec2* vec);

#ifdef __cplusplus
}
#endif
