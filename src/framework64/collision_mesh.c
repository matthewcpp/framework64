#include "framework64/collision_mesh.h"

#include "framework64/collision.h"

#include <limits.h>

int fw64_collision_mesh_load_from_datasource(fw64CollisionMesh* collision_mesh, fw64DataSource* data_source, fw64Allocator* allocator) {
    // read the header in order to determine data size
    fw64_data_source_read(data_source, collision_mesh, sizeof(fw64CollisionMesh), 1);

    uint32_t point_data_size = collision_mesh->point_count * sizeof(Vec3);
    uint32_t element_data_size = collision_mesh->element_count * sizeof(uint16_t);
    char* data_buffer = fw64_allocator_malloc(allocator, point_data_size + element_data_size);

    //read data buffer and update pointers
    fw64_data_source_read(data_source, data_buffer, 1, point_data_size + element_data_size);
    collision_mesh->points = (Vec3*)data_buffer;
    collision_mesh->elements = (uint16_t*)(data_buffer + point_data_size);

    return 1;
}

void fw64_collision_mesh_uninit(fw64CollisionMesh* collision_mesh, fw64Allocator* allocator) {
    // note the chunk of data for the mesh collider is allocated together so just need to free the start
    fw64_allocator_free(allocator, collision_mesh->points);
}

int fw64_collision_mesh_test_sphere(const fw64CollisionMesh* collision_mesh, fw64Transform* transform, Vec3* center, float radius, Vec3* out_point) {
    int result = 0;

    // transform query sphere into mesh local space
    Vec3 local_center;
    fw64_transform_inv_mult_point(transform, center, &local_center);
    float local_radius = radius; //todo: fix this

    // check for collision of sphere and mesh triangles
    float closest_distance = FLT_MAX;
    Vec3 closest_hit;

    Vec3* points = collision_mesh->points;
    uint16_t* elements = collision_mesh->elements;

    const uint32_t triangle_count = fw64_collision_mesh_get_triangle_count(collision_mesh);
    for (uint32_t i = 0; i < triangle_count; i++) {
        Vec3 local_hit;
        uint32_t index = i * 3;

        Vec3* a = points + elements[index];
        Vec3* b = points + elements[index + 1];
        Vec3* c = points + elements[index + 2];

        if (fw64_collision_test_sphere_triangle(&local_center, local_radius, a, b, c, &local_hit)) {
            float distance = vec3_distance_squared(&local_center, &local_hit);

            if (distance >= closest_distance){
                continue;
            }

            closest_distance = distance;
            closest_hit = local_hit;
            result = 1;
        }
    }

    // transform hit point back to world space
    if (result) {
        fw64_transform_mult_point(transform, &closest_hit, out_point);
    }

    return result;
}

int fw64_collision_mesh_test_ray(const fw64CollisionMesh* collision_mesh, fw64Transform* transform, Vec3* origin, Vec3* direction, Vec3* out_point, float* out_dist) {
    int result = 0;

    // transform the ray into local space
    Vec3 local_origin, local_direction;
    Quat inv_rotation = transform->rotation;
    quat_conjugate(&inv_rotation);

    fw64_transform_inv_mult_point(transform, origin, &local_origin);
    quat_transform_vec3(&inv_rotation, direction, &local_direction);

    // check for collision of ray and mesh triangles
    float closest_distance = FLT_MAX;
    Vec3 closest_hit;

    Vec3* points = collision_mesh->points;
    uint16_t* elements = collision_mesh->elements;

    const uint32_t triangle_count = fw64_collision_mesh_get_triangle_count(collision_mesh);

    for (uint32_t i = 0; i < triangle_count; i++) {
        Vec3 local_hit;
        float local_dist;
        uint32_t index = i * 3;

        Vec3* a = points + elements[index];
        Vec3* b = points + elements[index + 1];
        Vec3* c = points + elements[index + 2];

        if (fw64_collision_test_ray_triangle(&local_origin, &local_direction, a, b, c, &local_hit, &local_dist)) {

            if (local_dist >= closest_distance) {
                continue;
            }

            closest_distance = local_dist;
            closest_hit = local_hit;
            result = 1;
        }
    }

    // transform hit point back to world space
    if (result) {
        fw64_transform_mult_point(transform, &closest_hit, out_point);
        *out_dist = closest_distance; // todo: this probably needs to be scaled?
    }

    return result;
}
