#include "framework64/plane.h"

#include "framework64/types.h"

void fw64_plane_init(fw64Plane* plane, Vec3* normal, float dist) {
    plane->normal = *normal;
    plane->dist = dist;
}

void fw64_plane_set_values(fw64Plane* plane, float nx, float ny, float nz, float d) {
    plane->normal.x = nx;
    plane->normal.y = ny;
    plane->normal.z = nz;
    plane->dist = d;
}

void fw64_plane_set_3_points(fw64Plane* plane, Vec3* v1, Vec3* v2, Vec3* v3) {
    Vec3 aux1, aux2;

    vec3_subtract(v1, v2, &aux1);
    vec3_subtract(v3, v2, &aux2);

    vec3_cross(&aux2, &aux1, &plane->normal);
    vec3_normalize(&plane->normal);
    plane->dist = -vec3_dot(&plane->normal, v2);
}

void fw64_plane_normalize(fw64Plane* plane) {
    float mag = fw64_sqrtf(plane->normal.x * plane->normal.x + plane->normal.y * plane->normal.y + plane->normal.z * plane->normal.z);

    plane->normal.x /= mag;
    plane->normal.y /= mag;
    plane->normal.z/= mag;
    plane->dist /= mag;
}

float fw64_plane_signed_distance_to_point(fw64Plane* plane, Vec3* point) {
    return plane->dist + vec3_dot(&plane->normal, point);
}
