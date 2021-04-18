#include "framework64/entity.h"

#include "framework64/matrix.h"

#include <nusys.h>

void entity_init(Entity* entity, fw64Mesh* mesh) {
    transform_init(&entity->transform);
    entity->mesh = mesh;

    entity_refresh(entity);
}

void entity_refresh(Entity* entity) {
    float fmatrix[16];
    matrix_from_trs(fmatrix, &entity->transform.position, &entity->transform.rotation, &entity->transform.scale);
    
    guMtxF2L((float (*)[4])fmatrix, &entity->transform.matrix);

    if (entity->mesh) 
        matrix_transform_box(fmatrix, &entity->mesh->info.bounding_box, &entity->bounding);
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
    transform_forward(&camera->transform, &camera_forward);
    transform_up(&camera->transform, &camera_up);

    vec3_add(&target, &entity->transform.position, &camera_forward);
    transform_look_at(&entity->transform, &target, &camera_up);
}
