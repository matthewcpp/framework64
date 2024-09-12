#include "framework64/frustum.h"

#include "framework64/types.h"

/*
 * Plane extraction Logic:
 * https://www.gamedevs.org/uploads/fast-extraction-viewing-frustum-planes-from-world-view-projection-matrix.pdf
 * Frustum Intersec:
 * https://cgvr.cs.uni-bremen.de/teaching/cg_literatur/lighthouse3d_view_frustum_culling/
 */

void fw64_frustum_set_from_matrix(fw64Frustum* frustum, float* m) {
    fw64_plane_set_values(&frustum->planes[FW64_FRUSTUM_PLANE_LEFT], m[3] + m[0], m[7] + m[4], m[11] + m[8], m[15] + m[12]);
    fw64_plane_set_values(&frustum->planes[FW64_FRUSTUM_PLANE_RIGHT], m[3] - m[0], m[7] - m[4], m[11] - m[8], m[15] - m[12]);
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

    for (int i = 0; i < 6; i++) {
        float distance = fw64_plane_signed_distance_to_point(&frustum->planes[i] , center);

        if (distance < -radius)
            return FW64_FRUSTUM_RESULT_OUTSIDE;
        else if (distance < radius)
            result = FW64_FRUSTUME_RESULT_INTERSECT;
    }
    return result;
}

int fw64_frustum_intersects_box(fw64Frustum* frustum, Box* box) {
    int result = FW64_FRUSTUM_RESULT_INSIDE;

    Vec3 vmin, vmax;
    for (int i = 0; i < 6; i++) {
        fw64Plane* plane = &frustum->planes[i];

        if(plane->normal.x > 0) {
            vmin.x = box->min.x;
            vmax.x = box->max.x;
        } else {
            vmin.x = box->max.x;
            vmax.x = box->min.x;
        }
        // Y axis
        if(plane->normal.y > 0) {
            vmin.y = box->min.y;
            vmax.y = box->max.y;
        } else {
            vmin.y = box->max.y;
            vmax.y = box->min.y;
        }
        // Z axis
        if(plane->normal.z > 0) {
            vmin.z = box->min.z;
            vmax.z = box->max.z;
        } else {
            vmin.z = box->max.z;
            vmax.z = box->min.z;
        }

        if (fw64_plane_signed_distance_to_point(plane , &vmax) < 0.0f)
            return FW64_FRUSTUM_RESULT_OUTSIDE;

        if (fw64_plane_signed_distance_to_point(plane, &vmin) < 0.0f)
            result = FW64_FRUSTUME_RESULT_INTERSECT;
    }

    return result;
}
