#include "framework64/util/dynamic_vector.h"

#include <string.h>

void fw64_dynamic_vector_init(fw64DynamicVector* vector, uint32_t item_size, fw64Allocator* allocator) {
    vector->_data = NULL;
    vector->_allocator = allocator;
    vector->_size = 0;
    vector->_capacity = 0;
    vector->_item_size = item_size;
}

void fw64_dynamic_vector_uninit(fw64DynamicVector* vector) {
    if (vector->_data) {
        vector->_allocator->free(vector->_allocator, vector->_data);
    }
}

#define FW64_DYNAMIC_VECTOR_DEFAULT_CAPACITY 4

static void fw64_dynamic_vector_grow(fw64DynamicVector* vector) {
    if (vector->_capacity > 0) {
        uint16_t new_capacity = vector->_capacity * 2;
        char* new_data = vector->_allocator->malloc(vector->_allocator, new_capacity * vector->_item_size);
        memcpy(new_data, vector->_data, vector->_capacity * vector->_item_size);
        vector->_allocator->free(vector->_allocator, vector->_data);
        vector->_data = new_data;
        vector->_capacity = new_capacity;
    } else {
        vector->_capacity = FW64_DYNAMIC_VECTOR_DEFAULT_CAPACITY;
        vector->_data = vector->_allocator->malloc(vector->_allocator, vector->_capacity * vector->_item_size);
    }
}

void* fw64_dynamic_vector_emplace_back(fw64DynamicVector* vector) {
    uint32_t offset = vector->_size * vector->_item_size;

    if (vector->_size == vector->_capacity) {
        fw64_dynamic_vector_grow(vector);
    }

    vector->_size += 1;
    return vector->_data + offset;
}

void* fw64_dynamic_vector_push_back(fw64DynamicVector* vector, void* item) {
    char* new_item = fw64_dynamic_vector_emplace_back(vector);
    memcpy(new_item, item, vector->_item_size);

    return new_item;
}

void fw64_dynamic_vector_pop_back(fw64DynamicVector* vector) {
    if (vector->_size > 0) {
        vector->_size -= 1;
    }
}

void fw64_dynamic_vector_clear(fw64DynamicVector* vector) {
    vector->_size = 0;
}
