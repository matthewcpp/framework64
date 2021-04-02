#ifndef FW64_QUAD_H
#define FW64_QUAD_H

#include "framework64/color.h"
#include "framework64/mesh.h"
#include "framework64/sprite.h"

void textured_quad_create(Mesh* mesh, ImageSprite* texture);
void textured_quad_set_tex_coords(Mesh* mesh, int frame, float s, float t);

void quad_create(Mesh* mesh, s16 size, Color* color);

#endif