#include "framework64/static_vector.h"

#include "framework64/math.h"

#include <string.h>

void fw64_static_vector_init(fw64StaticVector* vector, size_t item_size, size_t item_capacity, fw64Allocator* allocator) {
    vector->item_size = (uint32_t)item_size;
    vector->item_capacity = (uint32_t)item_capacity;
    vector->item_count = 0;

    if (item_capacity) {
        // vector->data = fw64_allocator_malloc(allocator, item_size * item_capacity);
        vector->data = fw64_allocator_memalign(allocator, 8, item_size * item_capacity);
    } else {
        vector->data = NULL;
    }
}

void fw64_static_vector_resize(fw64StaticVector* vector, size_t count) {
    vector->item_count = fw64_mini(vector->item_capacity, (int)count);
}

void fw64_static_vector_uninit(fw64StaticVector* vector, fw64Allocator* allocator) {
    if (vector->data) {
        fw64_allocator_free(allocator, vector->data);
    }
}

void* fw64_static_vector_alloc_back(fw64StaticVector* vector) {
    if (vector->item_count == vector->item_capacity) {
        return NULL;
    }

    char* item = vector->data + (vector->item_count * vector->item_size);
    vector->item_count += 1;

    return item;
}

int fw64_static_vector_push_back(fw64StaticVector* vector, void* item) {
    if (vector->item_count == vector->item_capacity) {
        return 0;
    }

    char* dest = vector->data + (vector->item_count * vector->item_size);
    memcpy(dest, item, vector->item_size);
    vector->item_count += 1;

    return 1;
}

void* fw64_static_vector_get_back(fw64StaticVector* vector) {
    if (vector->item_count == 0) {
        return NULL;
    }

    return vector->data + ((vector->item_count - 1) * vector->item_size);
}

void fw64_static_vector_pop_back(fw64StaticVector* vector) {
    if (vector->item_count > 0) {
        vector->item_count -= 1;
    }
}

void fw64_static_vector_clear(fw64StaticVector* vector) {
    vector->item_count = 0;
}

int fw64_static_vector_is_empty(fw64StaticVector* vector) {
    return vector->item_count == 0;
}

void* fw64_static_vector_get_item(fw64StaticVector* vector, uint32_t index) {
    return vector->data + (vector->item_size * index);
}
