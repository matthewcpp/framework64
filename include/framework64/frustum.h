#pragma once

#include "framework64/box.h"
#include "framework64/plane.h"

typedef enum {
    FW64_FRUSTUM_PLANE_LEFT,
    FW64_FRUSTUM_PLANE_RIGHT,
    FW64_FRUSTUM_PLANE_TOP,
    FW64_FRUSTUM_PLANE_BOTTOM,
    FW64_FRUSTUM_PLANE_NEAR,
    FW64_FRUSTUM_PLANE_FAR
} fw64FrustumPlane;

typedef enum {
    FW64_FRUSTUM_RESULT_OUTSIDE,
    FW64_FRUSTUME_RESULT_INTERSECT,
    FW64_FRUSTUM_RESULT_INSIDE
} fw64FrustumTestResult;

typedef struct {
    fw64Plane planes[6];
} fw64Frustum;

#ifdef __cplusplus
extern "C" {
#endif

void fw64_frustum_set_from_matrix(fw64Frustum* frustum, float* matrix);
fw64FrustumTestResult fw64_frustum_test_point(fw64Frustum* frustum, Vec3* point);
fw64FrustumTestResult fw64_frustum_test_sphere(fw64Frustum* frustum, Vec3* center, float radius);
int fw64_frustum_intersects_box(fw64Frustum* frustum, Box* box);


#ifdef __cplusplus
}
#endif
