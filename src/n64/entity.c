#include "framework64/entity.h"

#include "framework64/matrix.h"

#include <nusys.h>

void entity_init(Entity* entity, Mesh* mesh) {
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

void entity_set_mesh(Entity* entity, Mesh* mesh) {
    entity->mesh = mesh;

    if (mesh) {
        entity_refresh(entity);
    }
    else {
        entity->bounding.min = entity->transform.position;
        entity->bounding.max = entity->transform.position;
    }
}