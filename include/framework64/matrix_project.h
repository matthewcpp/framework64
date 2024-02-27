#pragma once

/** \file project.h */

#include "framework64/vec2.h"
#include "framework64/vec3.h"
#include "framework64/matrix.h"

/** Map the specified object coordinates into window coordinates. */
int fw64_matrix_project(Vec3* pt, float* modelview, float* projection, IVec2* view_size, Vec3* result);

/** Map the specified window coordinates into object coordinates. */
int fw64_matrix_unproject(Vec3* pt, float* modelview, float* projection, IVec2* view_pos, IVec2* view_size, Vec3* result);