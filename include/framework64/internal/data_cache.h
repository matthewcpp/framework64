#pragma once

#include <stdint.h>

#define FW64_ASSET_CACHE_INVALID_INDEX -1

typedef struct {
    uint32_t index;
    int count;
    void* data;
} fw64CachedData;

typedef struct {
    fw64CachedData* cached_data;
    uint32_t count;
    uint32_t capacity;
} fw64DataCache;


#ifdef __cplusplus
extern "C" {
#endif

void fw64_data_cache_init(fw64DataCache* cache);
void fw64_data_cache_uninit(fw64DataCache* cache);

void fw64_data_cache_add(fw64DataCache* cache, uint32_t index, void* data);
void* fw64_data_cache_retain(fw64DataCache* cache, uint32_t index);
int fw64_data_cache_release(fw64DataCache* cache, void* data);

#ifdef __cplusplus
}
#endif
