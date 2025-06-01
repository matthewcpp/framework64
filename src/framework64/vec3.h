#pragma once

/** \file vec3.h */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    float x, y, z;
} Vec3;

void vec3_set(Vec3* v, float x, float y, float z);
void vec3_set_all(Vec3* v, float val);
void vec3_copy(const Vec3* src, Vec3* dest);
int vec3_equals(const Vec3* a, const Vec3* b, float epsilon);

void vec3_add(const Vec3* a, const Vec3* b, Vec3* out);

/**
 * Adds two vec3's after scaling the second operand by a scalar value
 */
void vec3_add_and_scale(const Vec3* a,const  Vec3* b, float k, Vec3* out);
void vec3_subtract(const Vec3* a, const Vec3* b, Vec3* out);
void vec3_scale(const Vec3* v, float k, Vec3* out);
float vec3_distance(const Vec3* a, const Vec3* b);
float vec3_distance_squared(const Vec3* a, const Vec3* b);
void vec3_normalize(Vec3* v);
void vec3_negate(Vec3* v);
void vec3_cross(const Vec3* a, const Vec3* b, Vec3* out);
float vec3_dot(const Vec3* a, const Vec3* b);
float vec3_length(const Vec3* v);

void vec3_smoothstep(const Vec3* a, const Vec3* b, float t, Vec3* out);
void vec3_lerp(const Vec3* a, const Vec3* b, float t, Vec3* out);

#define vec3_forward() {0.0f, 0.0f, -1.0f}
#define vec3_back() {0.0f, 0.0f, 1.0f}
#define vec3_right() {1.0f, 0.0f, 0.0f}
#define vec3_left() {-1.0f, 0.0f, 0.0f}
#define vec3_up() {0.0f, 1.0f, 0.0f}
#define vec3_down() {0.0f, -1.0f, 0.0f}

#define vec3_zero() {0.0f, 0.0f, 0.0f}
#define vec3_one() {1.0f, 1.0f, 1.0f}

#define vec3_set_zero(vec) vec3_set((vec), 0.0f, 0.0f, 0.0f)
#define vec3_set_one(vec) vec3_set((vec), 1.0f, 1.0f, 1.0f)

#ifdef __cplusplus
}
#endif

