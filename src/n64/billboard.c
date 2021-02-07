#include "framework64/billboard.h"

#include <string.h>

const Vtx billboard_quad_1x1_verts[] = {
    {{ {-1, 1, 0}, 0, {0, 0}, {255, 0, 0, 255} }},
    {{ {1, 1, 0}, 0, {2048, 0}, {0, 255, 0, 255}  }},
    {{ {1, -1, 0}, 0, {2048, 2048}, {0, 0, 255, 255} }},
    {{ {-1, -1, 0}, 0, {0, 2048}, {255, 255, 0, 255} }},
};

const Vtx billboard_quad_2x2_verts[] = {
    {{ {-1, 1, 0}, 0, {0, 0}, {255, 0, 0, 255} }},
    {{ {0, 1, 0}, 0, {2048, 0}, {0, 255, 0, 255}  }},
    {{ {0, 0, 0}, 0, {2048, 2048}, {0, 0, 255, 255} }},
    {{ {-1, 0, 0}, 0, {0, 2048}, {255, 255, 0, 255} }},

    {{ {0, 1, 0}, 0, {0, 0}, {255, 0, 0, 255} }},
    {{ {1, 1, 0}, 0, {2048, 0}, {0, 255, 0, 255}  }},
    {{ {1, 0, 0}, 0, {2048, 2048}, {0, 0, 255, 255} }},
    {{ {0, 0, 0}, 0, {0, 2048}, {255, 255, 0, 255} }},

    {{ {-1, 0, 0}, 0, {0, 0}, {255, 0, 0, 255} }},
    {{ {0, 0, 0}, 0, {2048, 0}, {0, 255, 0, 255}  }},
    {{ {0, -1, 0}, 0, {2048, 2048}, {0, 0, 255, 255} }},
    {{ {-1, -1, 0}, 0, {0, 2048}, {255, 255, 0, 255} }},

    {{ {0, 0, 0}, 0, {0, 0}, {255, 0, 0, 255} }},
    {{ {1, 0, 0}, 0, {2048, 0}, {0, 255, 0, 255}  }},
    {{ {1, -1, 0}, 0, {2048, 2048}, {0, 0, 255, 255} }},
    {{ {0, -1, 0}, 0, {0, 2048}, {255, 255, 0, 255} }},
};

void billboard_quad_init(BillboardQuad* quad, BillboardQuadType type, ImageSprite* sprite) {
    transform_init(&quad->transform);
    guMtxIdent(&quad->dl_matrix);

    quad->type = type;
    quad->sprite = sprite;
    quad->frame = 0;

    switch (quad->type) {
        case BILLBOARD_QUAD_1X1:
            quad->vertices = &billboard_quad_1x1_verts[0];
            break;
        
        case BILLBOARD_QUAD_2X2:
            quad->vertices = &billboard_quad_2x2_verts[0];
            break;
    }
}

void billboard_quad_look_at_camera(BillboardQuad* quad, Camera* camera) {
    Vec3 camera_forward, camera_up, target;
    transform_forward(&camera->transform, &camera_forward);
    transform_up(&camera->transform, &camera_up);

    vec3_add(&target, &quad->transform.position, &camera_forward);
    transform_look_at(&quad->transform, &target, &camera_up);
}