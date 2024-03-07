#pragma once

#include "framework64/n64/material.h"

#include <nusys.h>

void fw64_n64_vertex_set_position_f(Vtx* vertex, float x, float y, float z);
void fw64_n64_vertex_set_normal_f(Vtx* vertex, float x, float y, float z);
void fw64_n64_vertex_set_color_rgba8(Vtx* vertex, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void fw64_n64_vertex_set_texcoords_f(Vtx* vertex, fw64Material* material, float s, float t);