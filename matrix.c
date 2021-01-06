#include "matrix.h"

void matrix_from_trs(float* out, Vec3* v, Quat* q, Vec3* s) {

  float x2 = q->x + q->x;
  float y2 = q->y + q->y;
  float z2 = q->z + q->z;
  float xx = q->x * x2;
  float xy = q->x * y2;
  float xz = q->x * z2;
  float yy = q->y * y2;
  float yz = q->y * z2;
  float zz = q->z * z2;
  float wx = q->w * x2;
  float wy = q->w * y2;
  float wz = q->w * z2;

  out[0] = (1 - (yy + zz)) * s->x;
  out[1] = (xy + wz) * s->x;
  out[2] = (xz - wy) * s->x;
  out[3] = 0;
  out[4] = (xy - wz) * s->y;
  out[5] = (1 - (xx + zz)) * s->y;
  out[6] = (yz + wx) * s->y;
  out[7] = 0;
  out[8] = (xz + wy) * s->z;
  out[9] = (yz - wx) * s->z;
  out[10] = (1 - (xx + yy)) * s->z;
  out[11] = 0;
  out[12] = v->x;
  out[13] = v->y;
  out[14] = v->z;
  out[15] = 1;
}