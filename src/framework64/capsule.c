#include "framework64/capsule.h"

static void fw64_capsule_update(fw64Capsule* capsule);

void fw64_capsule_init(fw64Capsule* capsule, const Vec3* base, const Vec3* tip, float radius){
    capsule->radius = radius;
    fw64_capsule_set_points(capsule, base, tip);
}

void fw64_capsule_set_points(fw64Capsule* capsule, const Vec3* base, const Vec3* tip) {
    capsule->base = *base;
    capsule->tip = *tip;

    fw64_capsule_update(capsule);
}

void fw64_capsule_update(fw64Capsule* capsule) {
    // cache the capsule axis: normalized direction from base -> tip
    Vec3 axis;
    vec3_subtract(&capsule->tip, &capsule->base, &axis);
    vec3_normalize(&axis);

    // compute points a and b, which are the centers of the spheres at each end of the capsule.
    Vec3 line_end_offset = axis;
    vec3_scale(&line_end_offset, capsule->radius, &line_end_offset);
    vec3_add(&capsule->base, &line_end_offset, &capsule->a);
    vec3_subtract(&capsule->tip, &line_end_offset, &capsule->b);

    // update the aabb: create a bounding box at points a and b, then merge them
    Vec3 extents = {capsule->radius, capsule->radius, capsule->radius};
    box_set_center_extents(&capsule->aabb, &capsule->a, &extents);

    Box bbox;
    box_set_center_extents(&capsule->aabb, &capsule->b, &extents);

    box_encapsulate_box(&capsule->aabb, &bbox);
}
