#pragma once

/** \file vec3.h */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    float x, y, z;
} Vec3;

void vec3_zero(Vec3* v);
void vec3_one(Vec3* v);
void vec3_set(Vec3* v, float x, float y, float z);
void vec3_set_all(Vec3* v, float val);
void vec3_copy(Vec3* dest, Vec3* src);
int vec3_equals(Vec3* a, Vec3* b, float epsilon);

void vec3_add(Vec3* out, Vec3* a, Vec3* b);

/**
 * Adds two vec3's after scaling the second operand by a scalar value
 */
void vec3_add_and_scale(Vec3* out, Vec3* a, Vec3* b, float k);
void vec3_subtract(Vec3* out, Vec3* a, Vec3* b);
void vec3_scale(Vec3* out, Vec3* v, float k);
float vec3_distance(Vec3* a, Vec3* b);
float vec3_distance_squared(Vec3* a, Vec3* b);
void vec3_normalize(Vec3* v);
void vec3_negate(Vec3* v);
void vec3_cross(Vec3* out, Vec3* a, Vec3* b);
float vec3_dot(Vec3* a, Vec3* b);

void vec3_smoothstep(Vec3* out, Vec3* a, Vec3* b, float t);
void vec3_lerp(Vec3* out, Vec3* a, Vec3* b, float t);

#ifdef __cplusplus
}
#endif

