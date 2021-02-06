#ifndef BILLBOARD_H
#define BILLBOARD_H

#include "framework64/camera.h"
#include "framework64/sprite.h"
#include "framework64/transform.h"

#include <nusys.h>

typedef struct {
    Transform transform;
    Mtx dl_matrix;
    Vtx vertices[4];
    ImageSprite* sprite;
    int frame;
} BillboardQuad;

void billboard_quad_init(BillboardQuad* quad, ImageSprite* sprite, int frame);
void billboard_quad_set_sprite(BillboardQuad* quad, ImageSprite* sprite, int frame);
void billboard_quad_look_at_camera(BillboardQuad* quad, Camera* camera);

#endif