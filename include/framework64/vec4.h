#pragma once

/** \file vec4.h */

typedef struct {
    float x, y, z, w;
} Vec4;

#ifdef __cplusplus
extern "C" {
#endif

void vec4_set(Vec4* v, float x, float y, float z, float w);
void vec4_div(Vec4* v, float d, Vec4* out);

#ifdef __cplusplus
}
#endif
