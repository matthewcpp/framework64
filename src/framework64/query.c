#include "framework64/query.h"

void fw64_intersect_moving_box_query_init(fw64IntersectMovingBoxQuery* query) {
    query->count = 0;
}

void fw64_intersect_moving_sphere_query_init(fw64IntersectMovingSphereQuery* query) {
    query->count = 0;
}
