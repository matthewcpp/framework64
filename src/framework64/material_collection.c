#include "framework64/material_collection.h"

#include <string.h>

void fw64_material_collection_init_empty(fw64MaterialCollection* collection, uint32_t size, fw64Allocator* allocator) {
    collection->size = size;
    collection->materials = fw64_allocator_malloc(allocator, sizeof(fw64Material*) * size);
    memset(collection->materials, 0, sizeof(fw64Material*) * size);
}

void fw64_material_collection_init_clone(fw64MaterialCollection* collection, const fw64MaterialCollection* source, fw64Allocator* allocator) {
    collection->size = source->size;
    collection->materials = fw64_allocator_malloc(allocator, sizeof(fw64Material*) * source->size);

    for (uint32_t i = 0; i < source->size; i++ ) {
        collection->materials[i] = fw64_material_clone(fw64_material_collection_get_material(source, i), allocator);
    }
}

void fw64_material_collection_uninit(fw64MaterialCollection* collection, fw64Allocator* allocator) {
    fw64_allocator_free(allocator, collection->materials);
    collection->size = 0;
}
