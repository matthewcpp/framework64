#pragma once

/** \file vec2.h */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    float x, y;
} Vec2;

void vec2_set(Vec2 *v, float x, float y);
void vec2_add(const Vec2* a, const Vec2* b, Vec2* out);
void vec2_scale(const Vec2* v, float k, Vec2* out);

typedef struct {
    int x, y;
} IVec2;

void ivec2_set(IVec2* vec, int x, int y);
void ivec2_add(const IVec2 *a, const IVec2 *b, IVec2* out);

#define ivec2_zero() {0, 0}

#ifdef __cplusplus
}
#endif
