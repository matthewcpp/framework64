#include "framework64/util/bump_allocator.h"

#include "framework64/types.h"

#include <string.h>
#include <stdint.h>

#define ALIGN_SIZE 8

static void* fw64_bump_allocator_malloc(fw64Allocator* allocator, size_t size) {
    fw64BumpAllocator* bump = (fw64BumpAllocator*)allocator;

    size_t allocated_size = size + ALIGN_SIZE - 1;
    allocated_size -= (allocated_size % ALIGN_SIZE);

    uintptr_t used = bump->next - bump->start;
    if (allocated_size > (bump->size - used))
        return NULL;

    bump->previous = bump->next;
    bump->next += allocated_size;
    return bump->previous;
}

static void* fw64_bump_allocator_memalign(fw64Allocator* allocator, size_t align, size_t size) {
    fw64BumpAllocator* bump = (fw64BumpAllocator*)allocator;

    uintptr_t aligned = ((uintptr_t)bump->next + (align - 1)) & ~(align - 1);
    bump->next = (char*)aligned;

    return fw64_bump_allocator_malloc(allocator, size);
}

static void fw64_bump_allocator_free(fw64Allocator* allocator, void* ptr) {
    fw64BumpAllocator* bump = (fw64BumpAllocator*)allocator;

    if (ptr == bump->previous) {
        bump->next = bump->previous;
    }
}

// if the buffer to grow was the last chunk allocated then we can just increase the pointer
// otherwise need to allocate a whole new buffer
static void* fw64_bump_allocator_realloc(fw64Allocator* allocator, void* ptr, size_t size) {
    if (size == 0)
        fw64_bump_allocator_free(allocator, ptr);

    fw64BumpAllocator* bump = (fw64BumpAllocator*)allocator;

    if (ptr == bump->previous) {
        size_t previous_size = bump->next - bump->previous;
        if (size > previous_size) {
            bump->next += size - previous_size;
        }

        return ptr;
    }
    else {
        void* data = fw64_bump_allocator_malloc(allocator, size);
        memcpy(data, ptr, size);
        return data;
    }
}

void fw64_bump_allocator_init(fw64BumpAllocator* bump, size_t size) {
    bump->interface.malloc = fw64_bump_allocator_malloc;
    bump->interface.memalign = fw64_bump_allocator_memalign;
    bump->interface.free = fw64_bump_allocator_free;
    bump->interface.realloc = fw64_bump_allocator_realloc;

    bump->size = size;
    bump->start = fw64_memalign(8, size);
    bump->next = bump->start;
    bump->previous = bump->start;
}

void fw64_bump_allocator_reset(fw64BumpAllocator* bump) {
    bump->next = bump->start;
    bump->previous = bump->start;
}

void fw64_bump_allocator_uninit(fw64BumpAllocator* bump) {
    fw64_free(bump->start);
}