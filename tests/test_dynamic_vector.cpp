#include "framework64/util/dynamic_vector.h"

#include <gtest/gtest.h>

class DynamicVectorTest : public ::testing::Test {
protected:
    struct TestPoint{
        float x, y;
    };
    fw64DynamicVector vector;

    void SetUp() override {
        fw64_default_allocator_init();
    }

    void TearDown() override {
        fw64_dynamic_vector_uninit(&vector);
    }
};

TEST_F(DynamicVectorTest, Init) {
    fw64_dynamic_vector_init(&vector, sizeof(TestPoint), fw64_default_allocator());
    
    ASSERT_EQ(vector._allocator, fw64_default_allocator());
    ASSERT_TRUE(fw64_dynamic_vector_is_empty(&vector));
    ASSERT_EQ(fw64_dynamic_vector_size(&vector), 0);
    ASSERT_EQ(fw64_dynamic_vector_capacity(&vector), 0);
    ASSERT_EQ(fw64_dynamic_vector_item_size(&vector), sizeof(TestPoint));
}

TEST_F(DynamicVectorTest, PushBack) {
    const size_t POINT_COUNT = 10;
    TestPoint test_points[POINT_COUNT];
    for (size_t i = 0; i < POINT_COUNT; i++) {
        test_points[i].x = static_cast<float>(i);
        test_points[i].y = static_cast<float>(i * 10);
    }

    fw64_dynamic_vector_init(&vector, sizeof(TestPoint), fw64_default_allocator());

    for (size_t i = 0; i < POINT_COUNT; i++) {
        TestPoint* copied_pt = (TestPoint*)fw64_dynamic_vector_push_back(&vector, &test_points[i]);
        ASSERT_FLOAT_EQ(test_points[i].x, copied_pt->x) << "push_back x:" << i;
        ASSERT_FLOAT_EQ(test_points[i].y, copied_pt->y) << "push_back y:" << i;
    }

    ASSERT_FALSE(fw64_dynamic_vector_is_empty(&vector));
    ASSERT_EQ(fw64_dynamic_vector_size(&vector), POINT_COUNT);
    ASSERT_EQ(fw64_dynamic_vector_capacity(&vector), 16);
}

TEST_F(DynamicVectorTest, PopBack) {
    const size_t POINT_COUNT = 10;

    fw64_dynamic_vector_init(&vector, sizeof(TestPoint), fw64_default_allocator());

    fw64_dynamic_vector_pop_back(&vector);
    ASSERT_EQ(fw64_dynamic_vector_size(&vector), 0);

    for (size_t i = 0; i < POINT_COUNT; i++) {
        fw64_dynamic_vector_emplace_back(&vector);
    }

    for (size_t i = 0; i < POINT_COUNT; i++) {
        fw64_dynamic_vector_pop_back(&vector);
    }

    ASSERT_TRUE(fw64_dynamic_vector_is_empty(&vector));
    ASSERT_EQ(fw64_dynamic_vector_capacity(&vector), 16);
}

TEST_F(DynamicVectorTest, Clear) {
    const size_t POINT_COUNT = 10;

    fw64_dynamic_vector_init(&vector, sizeof(TestPoint), fw64_default_allocator());

    for (size_t i = 0; i < POINT_COUNT; i++) {
        fw64_dynamic_vector_emplace_back(&vector);
    }

    fw64_dynamic_vector_clear(&vector);

    ASSERT_TRUE(fw64_dynamic_vector_is_empty(&vector));
    ASSERT_EQ(fw64_dynamic_vector_size(&vector), 0);
    ASSERT_EQ(fw64_dynamic_vector_capacity(&vector), 16);
}