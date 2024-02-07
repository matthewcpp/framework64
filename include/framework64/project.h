#pragma once

/** \file project.h */

#include "framework64/vec2.h"
#include "framework64/vec3.h"
#include "framework64/matrix.h"

int fw64_project_point(Vec3* pt, float* modelview, float* projection, IVec2* view_size, Vec3* result);