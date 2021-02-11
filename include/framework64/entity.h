#ifndef FW64_ENTITY_H
#define FW64_ENTITY_H

#include "framework64/transform.h"
#include "framework64/box.h"

#include <nusys.h>

typedef struct {
    Transform transform;
    Mtx dl_matrix;
    Box bounding;
} Entity;

void entity_init(Entity* entity);

#endif