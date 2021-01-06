#ifndef VEC3_H
#define VEC3_H

typedef struct {
    float x, y, z;
} Vec3;

void vec3_zero(Vec3* v);
void vec3_one(Vec3* v);

#endif
