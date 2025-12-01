#pragma once

#include "framework64/vec3.h"
#include "framework64/box.h"

typedef struct {
    Vec3 base, tip;
    float radius;
    // points representing the center of spheres at each end of the capsule
    Vec3 a, b;
    // cached bounding box used for fast rejections
    Box aabb;
} fw64Capsule;

#ifdef __cplusplus
extern "C" {
#endif

void fw64_capsule_init(fw64Capsule* capsule, const Vec3* base, const Vec3* tip, float radius);
void fw64_capsule_set_points(fw64Capsule* capsule, const Vec3* base, const Vec3* tip);

#ifdef __cplusplus
}
#endif
