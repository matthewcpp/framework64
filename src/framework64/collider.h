#pragma once

/** \file collider.h */

#include "framework64/box.h"
#include "framework64/collision_mesh.h"
#include "framework64/mesh.h"

typedef struct fw64Node fw64Node;

// This enum needs to be kept in sync wit pipeline/gltf/Node.js
typedef enum {
    FW64_COLLIDER_BOX,
    FW64_COLLIDER_COLLISION_MESH
} fw64ColliderType;

typedef enum {
    FW64_COLLIDER_FLAG_NONE,
    FW64_COLLIDER_FLAG_ACTIVE
} fw64ColliderFlags;

typedef struct {
    fw64ColliderType type;
    fw64Node* node;
    fw64CollisionMesh* collision_mesh;
    Box source;
    Box bounding;
    fw64ColliderFlags flags;
} fw64Collider;

#ifdef __cplusplus
extern "C" {
#endif

void fw64_collider_init_box(fw64Collider* collider, fw64Node* node, Box* box);
void fw64_collider_init_collision_mesh(fw64Collider* collider, fw64Node* node, fw64CollisionMesh* collision_mesh);
void fw64_collider_update(fw64Collider* collider);

int fw64_collider_test_sphere(fw64Collider* collider, Vec3* center, float radius, Vec3* out_point);
int fw64_collider_test_ray(fw64Collider* collider, Vec3* origin, Vec3* direction, Vec3* out_point, float* out_dist);
int fw64_collider_test_box(fw64Collider* collider, Box* box);

#define fw64_collider_activate(collider) ((collider)->flags |= FW64_COLLIDER_FLAG_ACTIVE)
#define fw64_collider_deactivate(collider) ((collider)->flags &= ~FW64_COLLIDER_FLAG_ACTIVE)
#define fw64_collider_is_active(collider) ((collider)->flags & FW64_COLLIDER_FLAG_ACTIVE)

#ifdef __cplusplus
}
#endif
