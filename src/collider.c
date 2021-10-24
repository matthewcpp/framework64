#include "framework64/collider.h"

#include "framework64/collide.h"

static void fw64_collider_update_box(fw64Collider* collider);
static void fw64_collider_update_mesh(fw64Collider* collider);

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

void fw64_collider_set_type_mesh(fw64Collider* collider,fw64MeshCollider* mesh_collider) {
    collider->type = FW64_COLLIDER_MESH;
    collider->source.mesh = mesh_collider;

    fw64_collider_update_mesh(collider);
}

static void fw64_collider_update_box_with_transform(fw64Transform* transform, Box* source, Box* target) {
#ifdef PLATFORM_N64
    float fmatrix[16];
    guMtxF2L((float (*)[4])fmatrix, &transform.matrix);

    matrix_transform_box(&fmatrix[0], source, target);
#else
    matrix_transform_box(&transform->matrix.m[0], source, target);
#endif
}

void fw64_collider_update_box(fw64Collider* collider) {
    fw64_collider_update_box_with_transform(collider->transform, &collider->source.box, &collider->bounding);
}

void fw64_collider_update_mesh(fw64Collider* collider) {
    fw64_collider_update_box_with_transform(collider->transform, &collider->source.mesh->box, &collider->bounding);
}

void fw64_collider_update(fw64Collider* collider) {
    switch (collider->type) {
        case FW64_COLLIDER_BOX:
            fw64_collider_update_box(collider);
            break;

        case FW64_COLLIDER_MESH:
            fw64_collider_update_mesh(collider);
            break;

        default:
            break;
    }
}

int fw64_collider_mesh_test_sphere(fw64Collider* collider, Vec3* center, float radius, Vec3* out_point) {
    // first test the world space bounding
    if (!fw64_collision_test_box_sphere(&collider->bounding, center, radius, out_point))
        return 0;

    int result = 0;

    // transform query sphere into mesh local space
    Vec3 local_center;
    fw64_transform_inv_mult_point(collider->transform, center, &local_center);
    float local_radius = radius; //todo: fix this

    // check for collision of sphere and mesh triangles
    float closest_distance = FLT_MAX;
    Vec3 closest_hit;

    Vec3* points = collider->source.mesh->points;
    uint16_t* elements = collider->source.mesh->elements;

    uint32_t triangle_count = collider->source.mesh->element_count / 3;

    for (uint32_t i = 0; i < triangle_count; i++) {
        Vec3 local_hit;
        uint32_t index = i * 3;

        Vec3* a = points + elements[index];
        Vec3* b = points + elements[index + 1];
        Vec3* c = points + elements[index + 2];

        if (fw64_collision_test_sphere_triangle(&local_center, local_radius, a, b, c, &local_hit)) {
            float distance = vec3_distance_squared(&local_center, &local_hit);

            if (distance >= closest_distance)
                continue;

            closest_distance = distance;
            closest_hit = local_hit;
            result = 1;
        }
    }

    // transform hit point back to world space
    if (result) {
        fw64_transform_mult_point(collider->transform, &closest_hit, out_point);
    }

    return result;
}

int fw64_mesh_collider_test_ray(fw64Collider* collider, Vec3* origin, Vec3* direction, Vec3* out_point, float* out_dist) {
    // first test the world space bounding
    if (!fw64_collision_test_ray_box(origin, direction, &collider->bounding, out_point, out_dist))
        return 0;

    int result = 0;

    // transform the ray into local space
    Vec3 local_origin, local_direction;
    Quat inv_rotation = collider->transform->rotation;
    quat_conjugate(&inv_rotation);

    fw64_transform_inv_mult_point(collider->transform, origin, &local_origin);
    quat_transform_vec3(&local_direction, &inv_rotation, direction);

    // check for collision of sphere and mesh triangles
    float closest_distance = FLT_MAX;
    Vec3 closest_hit;

    Vec3* points = collider->source.mesh->points;
    uint16_t* elements = collider->source.mesh->elements;

    uint32_t triangle_count = collider->source.mesh->element_count / 3;

    for (uint32_t i = 0; i < triangle_count; i++) {
        Vec3 local_hit;
        float local_dist;
        uint32_t index = i * 3;

        Vec3* a = points + elements[index];
        Vec3* b = points + elements[index + 1];
        Vec3* c = points + elements[index + 2];

        if (fw64_collision_test_ray_triangle(&local_origin, &local_direction, a, b, c, &local_hit, &local_dist)) {

            if (local_dist >= closest_distance)
                continue;

            closest_distance = local_dist;
            closest_hit = local_hit;
            result = 1;
        }
    }

    // transform hit point back to world space
    if (result) {
        fw64_transform_mult_point(collider->transform, &closest_hit, out_point);
        *out_dist = closest_distance; // todo: this probably needs to be scaled?
    }

    return result;
}

int fw64_collider_test_sphere(fw64Collider* collider, Vec3* center, float radius, Vec3* out_point) {
    switch (collider->type) {
        case FW64_COLLIDER_BOX:
            return fw64_collision_test_box_sphere(&collider->bounding, center, radius, out_point);

        case FW64_COLLIDER_MESH:
            return fw64_collider_mesh_test_sphere(collider, center, radius, out_point);

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