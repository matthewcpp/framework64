#include "framework64/vec3.h"
#include "framework64/types.h"

#include <gtest/gtest.h>

TEST(Vec3, Lerp) {
    Vec3 a = {20.0f, 40.0f, 100.0f};
    Vec3 b = {60.0f, 100.0f, 200.0f};

    Vec3 expected { 40.0f, 70.0f, 150.0f};
    Vec3 actual;

    vec3_lerp(&a, &b, 0.5f, &actual);

    ASSERT_TRUE(vec3_equals(&actual, &expected, EPSILON));
}