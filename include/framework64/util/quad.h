#ifndef FW64_QUAD_H
#define FW64_QUAD_H

#include "framework64/color.h"
#include "framework64/mesh.h"
#include "framework64/texture.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void textured_quad_create(fw64Mesh* mesh, fw64Texture* texture);
void textured_quad_set_tex_coords(fw64Mesh* mesh, int frame, float s, float t);

void quad_create(fw64Mesh* mesh, int16_t size, Color* color);

#ifdef __cplusplus
}
#endif

#endif