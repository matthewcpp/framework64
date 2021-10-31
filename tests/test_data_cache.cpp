#include "framework64/internal/data_cache.h"

#include <gtest/gtest.h>

class AssetCacheTest : public ::testing::Test {
protected:
    fw64DataCache asset_cache;

    void SetUp() override {
        fw64_data_cache_init(&asset_cache);
    }

    void TearDown() override {
        fw64_data_cache_uninit(&asset_cache);
    }
};

TEST_F(AssetCacheTest, Add) {
    int val = 5566;
    uint32_t expected_count = 10;

    for (uint32_t i = 0; i < expected_count; i++) {
        fw64_data_cache_add(&asset_cache, i, &val);
    }

    ASSERT_EQ(expected_count, asset_cache.count);

    for (uint32_t i = 0; i < expected_count; i++) {
        auto* cached_asset = asset_cache.cached_data + i;

        ASSERT_EQ(cached_asset->count, 1);
        ASSERT_EQ(cached_asset->index, i);
        ASSERT_EQ(cached_asset->data, &val);
    }
}
#define DEFAULT_CAP 16

TEST_F(AssetCacheTest, GrowBuffer) {
    int vals[DEFAULT_CAP +1];

    for (int i = 0; i < DEFAULT_CAP + 1; i++) {
        vals[i] = i * 20;
    }

    for (int i = 0; i < DEFAULT_CAP; i++) {
        fw64_data_cache_add(&asset_cache, i * 10, &vals[i]);
    }

    ASSERT_EQ(asset_cache.capacity, DEFAULT_CAP);
    fw64_data_cache_add(&asset_cache, DEFAULT_CAP * 10, &vals[DEFAULT_CAP]);

    ASSERT_EQ(asset_cache.count, DEFAULT_CAP + 1);
    ASSERT_EQ(asset_cache.capacity, DEFAULT_CAP * 2);

    for (uint32_t i = 0; i < asset_cache.count; i++) {
        auto* cached_asset = asset_cache.cached_data + i;

        ASSERT_EQ(cached_asset->count, 1);
        ASSERT_EQ(cached_asset->index, i * 10);
        ASSERT_EQ(cached_asset->data, &vals[i]);
    }
}

TEST_F(AssetCacheTest, RetainNotPresent) {
    int val1 = 5566;

    fw64_data_cache_add(&asset_cache, 0, &val1);
    ASSERT_EQ(asset_cache.count, 1);

    void* result = fw64_data_cache_retain(&asset_cache, 10);

    ASSERT_EQ(result, nullptr);

    auto* cached_asset = asset_cache.cached_data + 0;
    ASSERT_EQ(cached_asset->count, 1);
}

TEST_F(AssetCacheTest, RetainExisting) {
    int val1 = 5566;
    int val2 = 2187;

    fw64_data_cache_add(&asset_cache, 0, &val1);
    fw64_data_cache_add(&asset_cache, 1, &val2);

    ASSERT_EQ(asset_cache.count, 2);

    void* result1 = fw64_data_cache_retain(&asset_cache, 0);
    ASSERT_EQ(result1, &val1);
    result1 = fw64_data_cache_retain(&asset_cache, 0);
    ASSERT_EQ(result1, &val1);

    void* result2 = fw64_data_cache_retain(&asset_cache, 1);
    ASSERT_EQ(result2, &val2);

    fw64CachedData * cached_asset = asset_cache.cached_data + 0;
    ASSERT_EQ(cached_asset->count, 3);

    cached_asset = asset_cache.cached_data + 1;
    ASSERT_EQ(cached_asset->count, 2);
}

TEST_F(AssetCacheTest, ReleaseInvalid) {
    int val1 = 5566;
    int val2 = 2187;

    fw64_data_cache_add(&asset_cache, 0, &val1);

    ASSERT_EQ(asset_cache.count, 1);

    int result = fw64_data_cache_release(&asset_cache, &val2);

    ASSERT_EQ(result, FW64_ASSET_CACHE_INVALID_INDEX);
    auto cached_asset = asset_cache.cached_data + 0;
    ASSERT_EQ(cached_asset->count, 1);
}

TEST_F(AssetCacheTest, ReleaseDecrement) {
    int val1 = 5566;

    fw64_data_cache_add(&asset_cache, 0, &val1);
    ASSERT_EQ(asset_cache.count, 1);

    int retain_count = 5;
    for (int i = 0; i < retain_count; i++)
        fw64_data_cache_retain(&asset_cache, 0);

    auto* cached_asset = asset_cache.cached_data + 0;
    ASSERT_EQ(cached_asset->count, retain_count + 1);

    for (int i = 0; i < retain_count; i++) {
        int count = fw64_data_cache_release(&asset_cache, &val1);
        ASSERT_EQ(count, retain_count - i);
    }
}

TEST_F(AssetCacheTest, ReleaseLast) {
    int val = 5566, val2 = 2187, val3 = 1122;

    fw64_data_cache_add(&asset_cache, 10, &val);
    fw64_data_cache_add(&asset_cache, 20, &val2);
    fw64_data_cache_add(&asset_cache, 30, &val3);

    ASSERT_EQ(asset_cache.count, 3);

    int count = fw64_data_cache_release(&asset_cache, &val3);
    ASSERT_EQ(count, 0);
    ASSERT_EQ(asset_cache.count, 2);
}

TEST_F(AssetCacheTest, ReleaseFirst) {
    int val = 5566, val2 = 2187, val3 = 1122;

    fw64_data_cache_add(&asset_cache, 10, &val);
    fw64_data_cache_add(&asset_cache, 20, &val2);
    fw64_data_cache_add(&asset_cache, 30, &val3);

    ASSERT_EQ(asset_cache.count, 3);

    int count = fw64_data_cache_release(&asset_cache, &val);
    ASSERT_EQ(count, 0);
    ASSERT_EQ(asset_cache.count, 2);

    auto* cached_asset = asset_cache.cached_data + 0;
    ASSERT_EQ(cached_asset->count, 1);
    ASSERT_EQ(cached_asset->index, 30);
}

TEST_F(AssetCacheTest, ReleaseMiddle) {
    int val = 5566, val2 = 2187, val3 = 1122;

    fw64_data_cache_add(&asset_cache, 10, &val);
    fw64_data_cache_add(&asset_cache, 20, &val2);
    fw64_data_cache_add(&asset_cache, 30, &val3);

    ASSERT_EQ(asset_cache.count, 3);

    int count = fw64_data_cache_release(&asset_cache, &val2);
    ASSERT_EQ(count, 0);
    ASSERT_EQ(asset_cache.count, 2);

    auto* cached_asset = asset_cache.cached_data + 1;
    ASSERT_EQ(cached_asset->count, 1);
    ASSERT_EQ(cached_asset->index, 30);
}
