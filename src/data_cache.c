#include "framework64/internal/data_cache.h"

#include <stdlib.h>
#include <string.h>

#ifdef PLATFORM_N64
#include <malloc.h>
#endif

#define CAPACITY_STEP 16

void fw64_data_cache_init(fw64DataCache* cache) {
    cache->capacity = CAPACITY_STEP;
    cache->count = 0;
    cache->cached_data = malloc(sizeof(fw64CachedData) * cache->capacity);
}

void fw64_data_cache_uninit(fw64DataCache* cache) {
    free(cache->cached_data);
}

void* fw64_data_cache_retain(fw64DataCache* cache, uint32_t index) {
    for (uint32_t i = 0; i < cache->count; i++) {
        fw64CachedData* cached_asset = cache->cached_data + i;

        if (cached_asset->index == index) {
            cached_asset->count += 1;
            return cached_asset->data;
        }
    }

    return NULL;
}

void fw64_data_cache_add(fw64DataCache* cache, uint32_t index, void* data) {
    if (cache->count == cache->capacity) {
        cache->capacity += CAPACITY_STEP;
        cache->cached_data = realloc(cache->cached_data, sizeof(fw64CachedData) * cache->capacity);
    }

    fw64CachedData* asset = cache->cached_data + cache->count;
    asset->data = data;
    asset->index = index;
    asset->count = 1;

    cache->count += 1;
}

static void remove_cached_item(fw64DataCache* cache, uint32_t index) {
    fw64CachedData* to_remove = cache->cached_data + index;
    fw64CachedData* replacement = cache->cached_data + (cache->count - 1);

    if (to_remove != replacement) {
        memcpy(to_remove, replacement, sizeof(fw64CachedData));
    }

    cache->count -= 1;
}

int fw64_data_cache_release(fw64DataCache* cache, void* data) {
    int i;
    fw64CachedData* cached_asset = NULL;

    for (i = 0; i < cache->count; i++) {
        cached_asset = cache->cached_data + i;

        if (cached_asset->data == data) {
            break;
        }
    }

    // item not found
    if (i >= cache->count) {
        return FW64_ASSET_CACHE_INVALID_INDEX;
    }

    cached_asset->count -= 1;

    if (cached_asset->count > 0) {
        return cached_asset->count;
    }

    remove_cached_item(cache, i);

    return 0;
}
