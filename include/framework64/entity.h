#ifndef FW64_ENTITY_H
#define FW64_ENTITY_H

#include "framework64/box.h"
#include "framework64/camera.h"
#include "framework64/mesh.h"
#include "framework64/transform.h"

typedef struct {
    Transform transform;
    Box bounding;
    fw64Mesh* mesh;
} Entity;

void entity_init(Entity* entity, fw64Mesh* mesh);
void entity_set_mesh(Entity* entity, fw64Mesh* mesh);
void entity_refresh(Entity* entity);
void entity_billboard(Entity* entity, Camera* camera);

#endif