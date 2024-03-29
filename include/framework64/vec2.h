#pragma once

/** \file vec2.h */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    float x, y;
} Vec2;

void vec2_set(Vec2 *v, float x, float y);
void vec2_add(Vec2* out, Vec2* a, Vec2* b);
void vec2_scale(Vec2* out, Vec2* v, float k);

typedef struct {
    int x, y;
} IVec2;

void ivec2_set(IVec2* vec, int x, int y);
void ivec2_add(IVec2* out, IVec2 *a, IVec2 *b);

#ifdef __cplusplus
}
#endif
