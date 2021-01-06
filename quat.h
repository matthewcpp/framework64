#ifndef QUAT_H
#define QUAT_H

typedef struct {
    float x, y, z, w;
} Quat;

void quat_ident(Quat* q);
void quat_set_axis_angle(Quat* q, float x, float y, float z, float rad);

#endif
