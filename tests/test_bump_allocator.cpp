#include "framework64/util/bump_allocator.h"

#include <gtest/gtest.h>

#define BUMP_ALLOCATOR_TEST_SIZE (64 * 1024)

class BumpAllocatorTest : public ::testing::Test {
protected:
    fw64BumpAllocator bump_allocator;
    fw64Allocator* allocator;

    void SetUp() override {
        fw64_bump_allocator_init(&bump_allocator, BUMP_ALLOCATOR_TEST_SIZE);
        allocator = &bump_allocator.interface;
    }

    void TearDown() override {
        fw64_bump_allocator_uninit(&bump_allocator);
    }
};

TEST_F(BumpAllocatorTest, Init) {
    ASSERT_EQ(bump_allocator.size, BUMP_ALLOCATOR_TEST_SIZE);
    ASSERT_EQ(bump_allocator.next, bump_allocator.start);
    ASSERT_EQ(bump_allocator.previous, bump_allocator.start);
}

TEST_F(BumpAllocatorTest, Malloc) {
    #define ALLOCATION_SIZE 32
    void* data1 = fw64_allocator_malloc(allocator, ALLOCATION_SIZE);

    ASSERT_EQ(bump_allocator.previous, bump_allocator.start);
    ASSERT_EQ(data1, bump_allocator.previous);
    ASSERT_EQ( bump_allocator.next, bump_allocator.previous + ALLOCATION_SIZE);

    void* data2 = fw64_allocator_malloc(allocator, ALLOCATION_SIZE);

    ASSERT_EQ(bump_allocator.previous, data2);
    ASSERT_EQ( bump_allocator.next, bump_allocator.previous + ALLOCATION_SIZE);
}

TEST_F(BumpAllocatorTest, Malloc_Unaligned) {
    void* data1 = fw64_allocator_malloc(allocator, 11);
    void* data2 = fw64_allocator_malloc(allocator, 8);

    auto ptr_diff = reinterpret_cast<uintptr_t>(data2) - reinterpret_cast<uintptr_t>(data1);
    ASSERT_EQ(ptr_diff, 16);
}

TEST_F(BumpAllocatorTest, Memalign) {
    #define ALIGNMENT 8
    void* data1 = fw64_allocator_malloc(allocator, 8);
    void* data2 = fw64_allocator_memalign(allocator, ALIGNMENT, 32);

    uintptr_t stride = (uintptr_t)data2 - (uintptr_t)data1;
    ASSERT_EQ((uintptr_t)data2 % ALIGNMENT, 0);
    ASSERT_EQ(stride, ALIGNMENT);
}

TEST_F(BumpAllocatorTest, Free) {
    void* data1 = fw64_allocator_malloc(allocator, 64);
    fw64_allocator_malloc(allocator, 8);
    void* data3 = fw64_allocator_malloc(allocator, 128);

    // free the first allocation...nothing happens
    char* expected_next = bump_allocator.next;

    fw64_allocator_free(allocator, data1);
    ASSERT_EQ(bump_allocator.next, expected_next);


    // free the last allocation...the space can be reclaimed
    expected_next = bump_allocator.previous;
    fw64_allocator_free(allocator, data3);
    ASSERT_EQ(bump_allocator.next, expected_next);
}

TEST_F(BumpAllocatorTest, ReallocLast) {
    int* data = (int*)fw64_allocator_malloc(allocator, 10 * sizeof(int));

    for (int i = 0; i < 10; i++)
        data[i] = i * 10;

    int* data2 = (int*)fw64_allocator_realloc(allocator, data, 20 * sizeof(int));

    ASSERT_EQ(data, data2);

    for (int i = 0; i < 10; i++)
        data[i] = i * 10;
}

TEST_F(BumpAllocatorTest, Realloc) {
    int* data = (int*)fw64_allocator_malloc(allocator, 10 * sizeof(int));

    for (int i = 0; i < 10; i++)
        data[i] = i * 10;

    fw64_allocator_malloc(allocator, 16);
    int* data2 = (int*)fw64_allocator_realloc(allocator, data, 20 * sizeof(int));

    ASSERT_NE(data, data2);

    for (int i = 0; i < 10; i++)
        ASSERT_EQ(data[i], data2[i]);
}

TEST_F(BumpAllocatorTest, Clear) {
    fw64_allocator_malloc(allocator, 2);
    fw64_allocator_malloc(allocator, 4);
    fw64_allocator_malloc(allocator, 8);

    ASSERT_NE(bump_allocator.next, bump_allocator.start);
    ASSERT_NE(bump_allocator.previous, bump_allocator.start);

    fw64_bump_allocator_reset(&bump_allocator);

    ASSERT_EQ(bump_allocator.next, bump_allocator.start);
    ASSERT_EQ(bump_allocator.previous, bump_allocator.start);

}
