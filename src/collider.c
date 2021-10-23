#include "framework64/collider.h"

#include "framework64/collide.h"

static void fw64_collider_update_box(fw64Collider* collider);

void fw64_collider_init(fw64Collider* collider, fw64Transform* transform) {
    collider->type = FW64_COLLIDER_NONE;
    collider->transform = transform;
}

void fw64_collider_set_type_none(fw64Collider* collider) {
    collider->type = FW64_COLLIDER_NONE;
}

void fw64_collider_set_type_box(fw64Collider* collider, Box* box) {
    collider->type = FW64_COLLIDER_BOX;
    collider->source.box = *box;

    fw64_collider_update_box(collider);
}

//TODO: Implementation
void fw64_collider_set_type_mesh(fw64Collider* collider, fw64Mesh* mesh) {
    collider->type = FW64_COLLIDER_NONE;
}


void fw64_collider_update_box(fw64Collider* collider) {
#ifdef PLATFORM_N64
    float fmatrix[16];
    guMtxF2L((float (*)[4])fmatrix, &node->transform.matrix);

    matrix_transform_box(&fmatrix[0], &collider->source.box, &collider->bounding);
#else
    matrix_transform_box(&collider->transform->matrix.m[0], &collider->source.box, &collider->bounding);
#endif
}

void fw64_collider_update(fw64Collider* collider) {
    switch (collider->type) {
        case FW64_COLLIDER_BOX:
            fw64_collider_update_box(collider);
            break;

        default:
            break;
    }
}

int fw64_collider_test_sphere(fw64Collider* collider, Vec3* center, float radius, Vec3* out_point) {
    switch (collider->type) {
        case FW64_COLLIDER_BOX:
            return fw64_collision_test_box_sphere(&collider->bounding, center, radius, out_point);
        default:
            return 0;
    }
}

int fw64_collider_test_ray(fw64Collider* collider, Vec3* origin, Vec3* direction, Vec3* out_point, float* out_dist) {
    switch (collider->type) {
        case FW64_COLLIDER_BOX:
            return fw64_collision_test_ray_box(origin, direction, &collider->bounding, out_point, out_dist);
        default:
            return 0;
    }
}