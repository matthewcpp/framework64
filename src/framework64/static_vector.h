#pragma once

/** \file static_vetor.h */

#include "framework64/allocator.h"

#include <stdint.h>

typedef struct {
    uint32_t item_size;
    uint32_t item_capacity;
    uint32_t item_count;
    char* data;
} fw64StaticVector;

#ifdef __cplusplus
extern "C" {
#endif

void fw64_static_vector_init(fw64StaticVector* vector, size_t item_size, size_t item_capacity, fw64Allocator* allocator);
void fw64_static_vector_uninit(fw64StaticVector* vector, fw64Allocator* allocator);
void fw64_static_vector_resize(fw64StaticVector* vector, size_t count);

void* fw64_static_vector_alloc_back(fw64StaticVector* vector);
int fw64_static_vector_push_back(fw64StaticVector* vector, void* item);
void* fw64_static_vector_get_back(fw64StaticVector* vector);
void fw64_static_vector_pop_back(fw64StaticVector* vector);

int fw64_static_vector_is_empty(fw64StaticVector* vector);
void fw64_static_vector_clear(fw64StaticVector* vector);

#define fw64_static_vector_size(vector) ((vector)->item_count)
void* fw64_static_vector_get_item(fw64StaticVector* vector, uint32_t index);

#ifdef __cplusplus
}
#endif
