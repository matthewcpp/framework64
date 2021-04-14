#include "test_sparse_set.h"

TEST_F(SparseSetTest, AllocItems) {
    fw64SparseSetHandle handles[4];

    for (uint16_t i = 0; i < 4; i++) {
        fw64_sparse_set_alloc(&sparse_set, &handles[i]);

        ASSERT_EQ(handles[i].lookup_index, i);
        ASSERT_EQ(handles[i].generation, 0);
    }

    for (uint16_t i = 0; i < 4; i++) {
        ASSERT_TRUE(fw64_sparse_set_handle_is_valid(&sparse_set, handles[i]));
    }
}

TEST_F(SparseSetTest, AllocGrowArray){
    fw64SparseSetHandle handles[10];

    for (uint16_t i = 0; i < 10; i++) {
        fw64_sparse_set_alloc(&sparse_set, &handles[i]);

        ASSERT_EQ(handles[i].lookup_index, i);
        ASSERT_EQ(handles[i].generation, 0);
    }

    for (uint16_t i = 0; i < 10; i++) {
        ASSERT_TRUE(fw64_sparse_set_handle_is_valid(&sparse_set, handles[i]));
    }
}

TEST_F(SparseSetTest, IsValid) {
    fw64SparseSetHandle handles[4];

    for (auto& handle : handles) {
        fw64_sparse_set_alloc(&sparse_set, &handle);
    }

    int delete_result = fw64_sparse_set_delete(&sparse_set, handles[1]);
    ASSERT_EQ(delete_result, 1);

    delete_result = fw64_sparse_set_delete(&sparse_set, handles[2]);
    ASSERT_EQ(delete_result, 1);

    ASSERT_TRUE(fw64_sparse_set_handle_is_valid(&sparse_set, handles[0]));
    ASSERT_FALSE(fw64_sparse_set_handle_is_valid(&sparse_set, handles[1]));
    ASSERT_FALSE(fw64_sparse_set_handle_is_valid(&sparse_set, handles[2]));
    ASSERT_TRUE(fw64_sparse_set_handle_is_valid(&sparse_set, handles[3]));
}

TEST_F(SparseSetTest, FreeList) {
    fw64SparseSetHandle handles[4];

    for (auto& handle : handles) {
        fw64_sparse_set_alloc(&sparse_set, &handle);
    }

    fw64_sparse_set_delete(&sparse_set, handles[1]);
    fw64_sparse_set_delete(&sparse_set, handles[2]);

    fw64SparseSetHandle actual_handle;
    fw64SparseSetHandle expected_handle = {.lookup_index = 2, .generation = 1};
    fw64_sparse_set_alloc(&sparse_set, &actual_handle);
    ASSERT_EQ(actual_handle, expected_handle);

    expected_handle = {.lookup_index = 1, .generation = 1};
    fw64_sparse_set_alloc(&sparse_set, &actual_handle);
    ASSERT_EQ(actual_handle, expected_handle);

    expected_handle = {.lookup_index = 4, .generation = 0};
    fw64_sparse_set_alloc(&sparse_set, &actual_handle);
    ASSERT_EQ(actual_handle, expected_handle);
}

TEST_F(SparseSetTest, DeleteOnlyItem) {
    fw64SparseSetHandle handle;
    fw64_sparse_set_alloc(&sparse_set, &handle);
    int result = fw64_sparse_set_delete(&sparse_set, handle);

    ASSERT_TRUE(result);
    ASSERT_EQ(sparse_set.item_count, 0);
    ASSERT_EQ(sparse_set.free_list, 0);
    ASSERT_FALSE(fw64_sparse_set_handle_is_valid(&sparse_set, handle));
}

TEST_F(SparseSetTest, GetItem) {
    fw64SparseSetHandle handles[10];

    for (int i = 0; i < 10; i++) {
        auto* player = reinterpret_cast<TestPlayer*>(fw64_sparse_set_alloc(&sparse_set, &handles[i]));

        player->health = i;
        player->pos_x = i * 2.0f;
        player->pos_y = i * 10.0f;
    }

    for (int i = 0; i < 10; i++) {
        auto* player = reinterpret_cast<TestPlayer*>(fw64_sparse_set_get_item(&sparse_set, handles[i]));

        ASSERT_EQ(player->health, i);
        ASSERT_EQ(player->pos_x, i * 2.0f);
        ASSERT_EQ(player->pos_y, i * 10.0f);
    }
}

// invalidates all existing tokens
TEST_F(SparseSetTest, Clear) {
    const int expected_size = 10;
    fw64SparseSetHandle handles[expected_size];

    for (auto & handle : handles) {
        fw64_sparse_set_alloc(&sparse_set, &handle);
        ASSERT_TRUE(fw64_sparse_set_handle_is_valid(&sparse_set, handle));
    }

    ASSERT_EQ(sparse_set.item_count, expected_size);
    fw64_sparse_set_clear(&sparse_set);
    ASSERT_EQ(sparse_set.item_count, 0);

    for (auto & handle : handles) {
        ASSERT_FALSE(fw64_sparse_set_handle_is_valid(&sparse_set, handle));
    }
}

TEST_F(SparseSetTest, IterateItems) {
    fw64SparseSetHandle handles[10];

    for (uint16_t i = 0; i < 10; i++) {
        TestPlayer* player = reinterpret_cast<TestPlayer*>(fw64_sparse_set_alloc(&sparse_set, &handles[i]));

        player->health = i;
        player->pos_x = i * 2.0f;
        player->pos_y = i * 10.0f;
    }

    uint16_t count = 0;

    fw64SparseSetItr itr;
    for (fw64_sparse_set_itr_init(&itr, &sparse_set); fw64_sparse_set_itr_has_next(&itr); fw64_sparse_set_itr_next(&itr)) {
        TestPlayer* player = reinterpret_cast<TestPlayer*>(fw64_sparse_set_itr_get_item(&itr));

        ASSERT_EQ(player->health, count);
        ASSERT_EQ(player->pos_x, count * 2.0f);
        ASSERT_EQ(player->pos_y, count * 10.0f);

        count += 1;
    }

    ASSERT_EQ(count, 10);
}
