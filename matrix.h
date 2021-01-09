#ifndef MATRIX_H
#define MATRIX_H

#include "vec3.h"
#include "quat.h"

void matrix_from_trs(float* out, Vec3* t, Quat* r, Vec3* s);
void matrix_target_to(float* out, Vec3* eye, Vec3* target, Vec3* up);
void matrix_get_scaling(float* mat, Vec3* out);
void matrix_get_rotation(float* mat, Quat* out);

#endif