#ifndef MATRIX_H
#define MATRIX_H

#include "vec3.h"
#include "quat.h"

void matrix_from_trs(float* m, Vec3* t, Quat* r, Vec3* s);

#endif