#ifndef FW64_QUAT_H
#define FW64_QUAT_H

#include "framework64/vec3.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    float x, y, z, w;
} Quat;

void quat_ident(Quat* q);
void quat_set(Quat* q, float x, float y, float z, float w);
void quat_set_axis_angle(Quat* q, float x, float y, float z, float rad);
void quat_transform_vec3(Vec3* out, Quat* q, Vec3* a);
void quat_normalize(Quat* q);

void quat_from_euler(Quat* q, float x, float y, float z);
#endif

#ifdef __cplusplus
}
#endif
