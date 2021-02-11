#ifndef FW64_BILLBOARD_H
#define FW64_BILLBOARD_H

#include "framework64/camera.h"
#include "framework64/sprite.h"
#include "framework64/transform.h"

#include <nusys.h>

typedef enum {
    BILLBOARD_QUAD_1X1,
    BILLBOARD_QUAD_2X2
} BillboardQuadType;

typedef struct {
    Transform transform;
    Mtx dl_matrix;
    const Vtx* vertices;
    ImageSprite* sprite;
    int frame;
    BillboardQuadType type;
} BillboardQuad;

void billboard_quad_init(BillboardQuad* quad, BillboardQuadType type, ImageSprite* sprite);
void billboard_quad_look_at_camera(BillboardQuad* quad, Camera* camera);

#endif