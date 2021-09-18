#include "framework64/frustum.h"

#include "framework64/types.h"

void fw64_frustum_set_from_matrix(fw64Frustum* frustum, float* m) {
    fw64_plane_set_values(&frustum->planes[FW64_FRUSTUM_PLANE_LEFT], m[3] - m[0], m[7] - m[4], m[11] - m[8], m[15] - m[12]);
    fw64_plane_set_values(&frustum->planes[FW64_FRUSTUM_PLANE_RIGHT], m[3] + m[0], m[7] + m[4], m[11] + m[8], m[15] + m[12]);
    fw64_plane_set_values(&frustum->planes[FW64_FRUSTUM_PLANE_BOTTOM], m[3] + m[1], m[7] + m[5], m[11] + m[9], m[15] + m[13]);
    fw64_plane_set_values(&frustum->planes[FW64_FRUSTUM_PLANE_TOP], m[3] - m[1], m[7] - m[5], m[11] - m[9], m[15] - m[13]);
    fw64_plane_set_values(&frustum->planes[FW64_FRUSTUM_PLANE_FAR], m[3] - m[2], m[7] - m[6], m[11] - m[10], m[15] - m[14]);
    fw64_plane_set_values(&frustum->planes[FW64_FRUSTUM_PLANE_NEAR], m[3] + m[2], m[7] + m[6], m[11] + m[10], m[15] + m[14]);

    fw64_plane_normalize(&frustum->planes[0]);
    fw64_plane_normalize(&frustum->planes[1]);
    fw64_plane_normalize(&frustum->planes[2]);
    fw64_plane_normalize(&frustum->planes[3]);
    fw64_plane_normalize(&frustum->planes[4]);
    fw64_plane_normalize(&frustum->planes[5]);
}

fw64FrustumTestResult fw64_frustum_test_point(fw64Frustum* frustum, Vec3* point) {
    for (int i = 0; i < 6; i++) {
        if (fw64_plane_signed_distance_to_point(&frustum->planes[i], point) < 0.0f)
            return FW64_FRUSTUM_RESULT_OUTSIDE;
    }

    return FW64_FRUSTUM_RESULT_INSIDE;
}

fw64FrustumTestResult fw64_frustum_test_sphere(fw64Frustum* frustum, Vec3* center, float radius) {
    int result = FW64_FRUSTUM_RESULT_INSIDE;

    for(int i = 0; i < 6; i++) {
        float distance = fw64_plane_signed_distance_to_point(&frustum->planes[i] , center);

        if (distance < -radius)
            return FW64_FRUSTUM_RESULT_OUTSIDE;
        else if (distance < radius)
            result = FW64_FRUSTUME_RESULT_INTERSECT;
    }
    return(result);
}

int fw64_frustum_intersects_box(fw64Frustum* frustum, Box* box) {
    return 0;
}
