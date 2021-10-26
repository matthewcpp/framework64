#pragma once

/** \file collider.h */

#include "framework64/box.h"
#include "framework64/mesh.h"
#include "framework64/transform.h"

typedef enum {
    FW64_COLLIDER_NONE,
    FW64_COLLIDER_BOX,
    FW64_COLLIDER_MESH
} fw64ColliderType;

typedef struct {
    uint32_t point_count;
    uint32_t element_count;
    Box box;
    Vec3* points;
    uint16_t* elements;
} fw64CollisionMesh;

typedef union {
    Box box;
    fw64CollisionMesh* mesh;
} fw64ColliderSource;

typedef struct {
    fw64ColliderType type;
    fw64Transform* transform;
    Box bounding;
    fw64ColliderSource source;
} fw64Collider;

#ifdef __cplusplus
extern "C" {
#endif

void fw64_collider_init(fw64Collider* collider, fw64Transform* transform);
void fw64_collider_set_type_none(fw64Collider* collider);
void fw64_collider_set_type_box(fw64Collider* collider, Box* box);
void fw64_collider_set_type_mesh(fw64Collider* collider, fw64CollisionMesh * collision_mesh);
void fw64_collider_update(fw64Collider* collider);

int fw64_collider_test_sphere(fw64Collider* collider, Vec3* center, float radius, Vec3* out_point);
int fw64_collider_test_ray(fw64Collider* collider, Vec3* origin, Vec3* direction, Vec3* out_point, float* out_dist);
int fw64_collider_test_box(fw64Collider* collider, Box* box);

#ifdef __cplusplus
}
#endif
