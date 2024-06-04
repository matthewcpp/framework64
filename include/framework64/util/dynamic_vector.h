#pragma once

#include "framework64/allocator.h"

#include <stddef.h>
#include <stdint.h>

typedef struct {
    char* _data;
    fw64Allocator* _allocator;
    uint16_t _size;
    uint16_t _capacity;
    uint32_t _item_size;
} fw64DynamicVector;

#ifdef __cplusplus
extern "C" {
#endif

void fw64_dynamic_vector_init(fw64DynamicVector* vector, uint32_t item_size, fw64Allocator* allocator);
void fw64_dynamic_vector_uninit(fw64DynamicVector* vector);

#define fw64_dynamic_vector_is_empty(vector) ((vector)->_size == 0)
#define fw64_dynamic_vector_size(vector) ((vector)->_size)
#define fw64_dynamic_vector_capacity(vector) ((vector)->_capacity)
#define fw64_dynamic_vector_item_size(vector) ((vector)->_item_size)
#define fw64_dynamic_vector_data(vector) ((vector)->_data)
#define fw64_dynamic_vector_item(vector, index) ((vector)->_data + ((vector)->_item_size * (index)))

void* fw64_dynamic_vector_push_back(fw64DynamicVector* vector, void* item);
void* fw64_dynamic_vector_emplace_back(fw64DynamicVector* vector);
void fw64_dynamic_vector_pop_back(fw64DynamicVector* vector);
void fw64_dynamic_vector_clear(fw64DynamicVector* vector);

#ifdef __cplusplus
}
#endif
