#pragma once

#include "framework64/color.h"
#include "framework64/n64/texture.h"

#include <nusys.h>

void fw64_n64_vertex_set_position_f(Vtx* vertex, float x, float y, float z);
void fw64_n64_vertex_set_position_s16(Vtx* vertex, short x, short y, short z);
void fw64_n64_vertex_set_normal_f(Vtx* vertex, float x, float y, float z);
void fw64_n64_vertex_set_color_rgba8(Vtx* vertex, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void fw64_n64_vertex_set_color(Vtx* vertex, fw64ColorRGBA8 color);
void fw64_n64_vertex_set_texcoords_f(Vtx* vertex, fw64Texture* texture, float s, float t);