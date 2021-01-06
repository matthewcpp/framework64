#ifndef ENTITY_H
#define ENTITY_H

#include "vec3.h"
#include "quat.h"

typedef struct {
    Vec3 position;
    Quat rotation;
    Vec3 scale;
} Entity;

void entity_init(Entity* entity);

#endif