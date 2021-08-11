#ifndef FW64_QUAD_H
#define FW64_QUAD_H

#include "framework64/color.h"
#include "framework64/mesh.h"
#include "framework64/texture.h"

void textured_quad_create(fw64Mesh* mesh, fw64Texture* texture);
void textured_quad_set_tex_coords(fw64Mesh* mesh, int frame, float s, float t);

void quad_create(fw64Mesh* mesh, s16 size, Color* color);

#endif