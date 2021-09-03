#include "entity.h"

#include "framework64/matrix.h"

static void _entity_update_bounding_box_with_matrix(Entity* entity, float* matrix) {
    if (entity->mesh) {
        Box mesh_bounding_box;
        fw64_mesh_get_bounding_box(entity->mesh, &mesh_bounding_box);
        matrix_transform_box(matrix, &mesh_bounding_box, &entity->bounding);
    }
}

void entity_init(Entity* entity, fw64Mesh* mesh) {
    fw64_transform_init(&entity->transform);
    entity->mesh = mesh;

    entity_refresh(entity);
}

void entity_refresh(Entity* entity) {
#ifdef PLATFORM_N64
    float fmatrix[16];
    matrix_from_trs(fmatrix, &entity->transform.position, &entity->transform.rotation, &entity->transform.scale);

    guMtxF2L((float (*)[4])fmatrix, &entity->transform.matrix);

    _entity_update_bounding_box_with_matrix(entity, &fmatrix[0]);
#else
    fw64_transform_update_matrix(&entity->transform);
    _entity_update_bounding_box_with_matrix(entity, &entity->transform.matrix.m[0]);
#endif
}

void entity_set_mesh(Entity* entity, fw64Mesh* mesh) {
    entity->mesh = mesh;

    if (mesh) {
        entity_refresh(entity);
    }
    else {
        entity->bounding.min = entity->transform.position;
        entity->bounding.max = entity->transform.position;
    }
}

void entity_billboard(Entity* entity, fw64Camera* camera) {
    Vec3 camera_forward, camera_up, target;
    fw64_transform_forward(&camera->transform, &camera_forward);
    fw64_transform_up(&camera->transform, &camera_up);

    vec3_add(&target, &entity->transform.position, &camera_forward);
    fw64_transform_look_at(&entity->transform, &target, &camera_up);
}
