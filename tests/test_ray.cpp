#include "framework64/raycast.h"
#include "framework64/types.h"

#include "gtest/gtest.h"

TEST(RayBox, Hit) {
    Box box = {{-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}};

    Vec3 origin = {0.0f, 10.0f, 0.0f};
    Vec3 direction = {0.0f, -1.0f, 0.0f};

    Vec3 hit_point;
    float t;

    int hit = fw64_collision_test_ray_box(&origin, &direction, &box, &hit_point, &t);

    Vec3 expected_point = {0.0f, 1.0f, 0.0f};
    float expected_distance = 9.0f;

    ASSERT_TRUE(hit);
    ASSERT_TRUE(vec3_equals(&hit_point, &expected_point, EPSILON));
    ASSERT_FLOAT_EQ(expected_distance, t);
}

TEST(RayBox, Miss) {
    Box box = {{-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}};

    Vec3 origin = {0.0f, 10.0f, 0.0f};
    Vec3 direction = {1.0f, 0.0f, 0.0f};

    Vec3 hit_point;
    float t;

    int hit = fw64_collision_test_ray_box(&origin, &direction, &box, &hit_point, &t);

    ASSERT_FALSE(hit);
}

TEST(RayTriangle, Hit) {
    Vec3 origin = {0.0f, 0.5f, 10.0f};
    Vec3 direction = {0.0f, 0.0f, -1.0f};

    Vec3 a = {-1.0f, -1.0f, 1.0f};
    Vec3 b = {0.0f, 1.0f, 1.0f};
    Vec3 c = {1.0f, -1.0f, 1.0f};

    Vec3 expected_point = {0.0f, 0.5f, 1.0f};
    float expected_distance = 9.0f;

    Vec3 hit_point;
    float t;

    int hit = fw64_collision_test_ray_triangle(&origin, &direction, &a, &b, &c, &hit_point, &t);

    ASSERT_TRUE(hit);
    ASSERT_TRUE(vec3_equals(&hit_point, &expected_point, EPSILON));
    ASSERT_FLOAT_EQ(expected_distance, t);
}

TEST(RayTriangle, Miss) {
    Vec3 origin = {0.0f, 0.5f, 10.0f};
    Vec3 direction = {0.0f, 0.0f, 1.0f};

    Vec3 a = {-1.0f, -1.0f, 1.0f};
    Vec3 b = {0.0f, 1.0f, 1.0f};
    Vec3 c = {1.0f, -1.0f, 1.0f};

    Vec3 hit_point;
    float t;

    int hit = fw64_collision_test_ray_triangle(&origin, &direction, &a, &b, &c, &hit_point, &t);

    ASSERT_FALSE(hit);
}