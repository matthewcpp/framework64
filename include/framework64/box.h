#ifndef FW64_BOX_H
#define FW64_BOX_H

#include "vec3.h"

typedef struct {
    Vec3 min;
    Vec3 max;
} Box;

void box_center(Box* box, Vec3* out);
void box_invalidate(Box* box);
void box_encapsulate_point(Box* box, Vec3* pt);
void box_encapsulate_box(Box* box, Box* to_encapsulate);

#endif