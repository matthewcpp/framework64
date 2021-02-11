#ifndef FW64_VEC3_H
#define FW64_VEC3_H

typedef struct {
    float x, y, z;
} Vec3;

void vec3_zero(Vec3* v);
void vec3_one(Vec3* v);
void vec3_set(Vec3* v, float x, float y, float z);

void vec3_add(Vec3* out, Vec3* a, Vec3* b);
void vec3_subtract(Vec3* out, Vec3* a, Vec3* b);
void vec3_scale(Vec3* out, Vec3* v, float k);
float vec3_distance(Vec3* a, Vec3* b);
void vec3_normalize(Vec3* v);
void vec3_negate(Vec3* v);

#endif
