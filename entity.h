#ifndef ENTITY_H
#define ENTITY_H

#include "transform.h"

#include <nusys.h>

typedef struct {
    Transform transform;
    Mtx dl_matrix;
} Entity;

void entity_init(Entity* entity);

#endif