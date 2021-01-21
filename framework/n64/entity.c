#include "ultra/entity.h"

#include <nusys.h>

void entity_init(Entity* entity) {
    transform_init(&entity->transform);
    guMtxIdent(&entity->dl_matrix);
}