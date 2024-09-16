#include "framework64/allocator.h"

#include "framework64/types.h"

void* fw64_allocator_malloc(fw64Allocator* allocator, size_t size) {
    return allocator->malloc(allocator, size);
}

void* fw64_allocator_realloc(fw64Allocator* allocator, void* ptr, size_t new_size) {
    return allocator->realloc(allocator, ptr, new_size);
}

void* fw64_allocator_memalign(fw64Allocator* allocator, size_t alignment, size_t size) {
    return allocator->memalign(allocator, alignment, size);
}

void  fw64_allocator_free(fw64Allocator* allocator, void* ptr) {
    allocator->free(allocator, ptr);
}

fw64Allocator default_allocator;

static void* fw64_default_allocator_malloc(fw64Allocator* allocator, size_t size) {
    (void)allocator;
    return fw64_malloc(size);
}

static void* fw64_default_allocator_realloc(fw64Allocator* allocator, void* ptr, size_t size) {
    (void)allocator;
    return fw64_realloc(ptr, size);
}

static void* fw64_default_allocator_memalign(fw64Allocator* allocator, size_t alignment, size_t size) {
    (void)allocator;
    (void)alignment;
    // TODO: Look into this why is fw64_memalign typedef'd to malloc on desktop?
    return fw64_memalign(alignment, size);
}

static void fw64_default_allocator_free(fw64Allocator* allocator, void* ptr) {
    (void)allocator;
    fw64_free(ptr);
}

void _fw64_default_allocator_init() {
    default_allocator.malloc = fw64_default_allocator_malloc;
    default_allocator.realloc = fw64_default_allocator_realloc;
    default_allocator.memalign = fw64_default_allocator_memalign;
    default_allocator.free = fw64_default_allocator_free;
}

fw64Allocator* fw64_default_allocator() {
    return &default_allocator;
}
