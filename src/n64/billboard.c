#include "framework64/billboard.h"

#include <string.h>

const Vtx default_billboard_verts[] = {
{ -1, 1, 0, 0, 0, 0, 255, 0, 0, 255 },
{ 1, 1, 0, 0, 0, 0, 0, 255, 0, 255 },
{ 1, -1, 0, 0, 0, 0, 0, 0, 255, 255 },
{ -1, -1, 0, 0, 0, 0, 255, 255, 0, 255 },
};

void billboard_quad_init(BillboardQuad* quad, ImageSprite* sprite, int frame) {
    transform_init(&quad->transform);
    guMtxIdent(&quad->dl_matrix);

    memcpy(&quad->vertices[0], &default_billboard_verts[0], sizeof(Vtx) * 4);
    billboard_quad_set_sprite(quad, sprite, frame);
}

void billboard_quad_set_sprite(BillboardQuad* quad, ImageSprite* sprite, int frame) {
    quad->sprite = sprite;
    quad->frame = frame;

    int width_coord = (image_sprite_get_slice_width(sprite) * 2) << 5;
    int height_coord = (image_sprite_get_slice_width(sprite) * 2) << 5;

    // adjust tex coords for vertex
    quad->vertices[1].v.tc[0] = width_coord;
    quad->vertices[2].v.tc[0] = width_coord;
    quad->vertices[2].v.tc[1] = height_coord;
    quad->vertices[3].v.tc[1] = height_coord;
}

void billboard_quad_look_at_camera(BillboardQuad* quad, Camera* camera) {
    Vec3 camera_forward, camera_up, target;
    transform_forward(&camera->transform, &camera_forward);
    transform_up(&camera->transform, &camera_up);

    vec3_add(&target, &quad->transform.position, &camera_forward);
    transform_look_at(&quad->transform, &target, &camera_up);
}