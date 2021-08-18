#ifndef FW64_VEC2_H
#define FW64_VEC2_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    float x, y;
} Vec2;

void vec2_set(Vec2 *v, float x, float y);

typedef struct {
    int x, y;
} IVec2;

void ivec2_add(IVec2 *out, IVec2 *a, IVec2 *b);

#ifdef __cplusplus
}
#endif

#endif