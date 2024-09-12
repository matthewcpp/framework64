#pragma once

/** \file plane.h */

#include "framework64/vec3.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    Vec3 normal;
    float dist;
} fw64Plane;

void fw64_plane_init(fw64Plane* plane, Vec3* normal, float dist);
void fw64_plane_set_values(fw64Plane* plane, float nx, float ny, float nz, float d);

// note points should be specified in counter clockwise order
void fw64_plane_set_3_points(fw64Plane* plane, Vec3* v1, Vec3* v2, Vec3* v3);

void fw64_plane_normalize(fw64Plane* plane);
float fw64_plane_signed_distance_to_point(fw64Plane* plane, Vec3* point);

#ifdef __cplusplus
}
#endif
