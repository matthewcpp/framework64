#include "framework64/collision.h"

#include <gtest/gtest.h>

static Box create_test_box(float cx, float cy, float cz, float ex, float ey, float ez);

TEST(MovingAABB_AABB, Hit) {
    Box box_a = create_test_box(0.0f, 0.0f, -20.0f, 5.0f, 5.0f, 5.0f);
    Vec3 box_a_vel = {0.0f, 0.0f, 0.0f};

    Box box_b = create_test_box(0.0f, 0.0f, 0.0f, 5.0f, 5.0f, 5.0f);
    Vec3 box_b_vel = {0.0, 0.0, -100.0f};

    float tfirst, tlast;
    int result = fw64_collision_test_moving_boxes(&box_a, &box_a_vel, &box_b, &box_b_vel, &tfirst, &tlast);

    ASSERT_EQ(result, 1);
}


Box create_test_box(float cx, float cy, float cz, float ex, float ey, float ez) {
    Vec3 center = {cx, cy, cz};
    Vec3 extents = {ex, ey, ez};
    Box b;
    box_set_center_extents(&b, &center, &extents);

    return b;
}