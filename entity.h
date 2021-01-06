#ifndef ENTITY_H
#define ENTITY_H

#include "quat.h"
#include "vec3.h"

#include <nusys.h>

typedef struct {
    Vec3 position;
    Quat rotation;
    Vec3 scale;
    Mtx dl_matrix;
} Entity;

void entity_init(Entity* entity);

#endif