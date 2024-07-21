#include "framework64/collider.h"

#include "framework64/collision.h"
#include "framework64/matrix.h"
#include "framework64/node.h"

#include <stddef.h>

static void fw64_collider_init(fw64Collider* collider, fw64Node* node, fw64ColliderType type, Box* box) {
    collider->type = type;
    collider->source = *box;
    collider->node = node;
    collider->flags = FW64_COLLIDER_FLAG_ACTIVE;

    node->collider = collider;

    fw64_collider_update(collider);
}

void fw64_collider_init_box(fw64Collider* collider, fw64Node* node, Box* box) {
    collider->collision_mesh = NULL;
    fw64_collider_init(collider, node, FW64_COLLIDER_BOX, box);
}

void fw64_collider_init_collision_mesh(fw64Collider* collider, fw64Node* node, fw64CollisionMesh * collision_mesh) {
    collider->collision_mesh = collision_mesh;
    fw64_collider_init(collider, node, FW64_COLLIDER_COLLISION_MESH, &collision_mesh->box);
}

void fw64_collider_update(fw64Collider* collider) {
    matrix_transform_box(collider->node->transform.world_matrix, &collider->source, &collider->bounding);
}

int fw64_collider_test_sphere(fw64Collider* collider, Vec3* center, float radius, Vec3* out_point) {
    int result = fw64_collision_test_box_sphere(&collider->bounding, center, radius, out_point);

    if (result && collider->type == FW64_COLLIDER_COLLISION_MESH) {
        result = fw64_collision_mesh_test_sphere(collider->collision_mesh, &collider->node->transform, center, radius, out_point);
    }

    return result;
}

int fw64_collider_test_ray(fw64Collider* collider, Vec3* origin, Vec3* direction, Vec3* out_point, float* out_dist) {
    int result = fw64_collision_test_ray_box(origin, direction, &collider->bounding, out_point, out_dist);

    if (result && collider->type == FW64_COLLIDER_COLLISION_MESH) {
        result = fw64_collision_mesh_test_ray(collider->collision_mesh, &collider->node->transform, origin, direction, out_point, out_dist);
    }

    return result;
}

int fw64_collider_test_box(fw64Collider* collider, Box* box) {
    return box_intersection(&collider->bounding, box);
}
