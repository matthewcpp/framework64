#include "entity.h"

#include <nusys.h>

void entity_init(Entity* entity) {
    vec3_zero(&entity->position);
    quat_ident(&entity->rotation);
    vec3_one(&entity->scale);

    guMtxIdent(&entity->dl_matrix);
}