#pragma once
#include "framework64/util/sparse_set.h"
#include "framework64/allocator.h"

#include "gtest/gtest.h"

#include <limits>

inline bool operator==(const fw64SparseSetHandle& a, const fw64SparseSetHandle& b) {
    return a.generation == b.generation && a.lookup_index == b.lookup_index;
}

class SparseSetTest : public ::testing::Test {
protected:
    struct TestPlayer{
        float pos_x;
        float pos_y;
        int health;
    };
    fw64SparseSet sparse_set;

    void SetUp() override {
        _fw64_default_allocator_init();
        fw64_sparse_set_init(&sparse_set, sizeof(TestPlayer), fw64_default_allocator());
    }

    void TearDown() override {
        fw64_sparse_set_uninit(&sparse_set);
    }
};