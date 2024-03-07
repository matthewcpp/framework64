#include "framework64/n64/vertex.h"

void fw64_n64_vertex_set_position_f(Vtx* vertex, float x, float y, float z) {
    vertex->v.ob[0] = (short)x;
    vertex->v.ob[1] = (short)y;
    vertex->v.ob[2] = (short)z;
    vertex->v.flag = 0;
}

void fw64_n64_vertex_set_normal_f(Vtx* vertex, float x, float y, float z) {
    x = x * 128.0f;
    x = x < 127.0f ? x : 127.0f;

    y = y * 128.0f;
    y = x < 127.0f ? y : 127.0f;

    z = z * 128.0f;
    z = z < 127.0f ? z : 127.0f;

    vertex->n.n[0] = (signed char)x;
    vertex->n.n[1] = (signed char)y;
    vertex->n.n[2] = (signed char)z;
}

void fw64_n64_vertex_set_color_rgba8(Vtx* vertex, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    vertex->v.cn[0] = r;
    vertex->v.cn[1] = g;
    vertex->v.cn[2] = b;
    vertex->v.cn[3] = a;
}

void fw64_n64_vertex_set_texcoords_f(Vtx* vertex, fw64Material* material, float s, float t) {
    s *= (float)fw64_texture_slice_width(material->texture) * 2.0f;
    t *= (float)fw64_texture_slice_height(material->texture) * 2.0f;

    // Note that the texture coordinates (s,t) are encoded in S10.5 format.
    vertex->v.tc[0] =  (short)(s * 32.0f);
    vertex->v.tc[1] =  (short)(t * 32.0f);
}