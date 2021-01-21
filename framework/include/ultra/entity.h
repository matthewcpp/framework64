#ifndef ENTITY_H
#define ENTITY_H

#include "ultra/transform.h"
#include "ultra/box.h"

#include <nusys.h>

typedef struct {
    Transform transform;
    Mtx dl_matrix;
    Box bounding;
} Entity;

void entity_init(Entity* entity);

#endif