#pragma once

/** \file bump_allocator.h */

#include "framework64/allocator.h"

#include <stdint.h>

typedef struct {
    fw64Allocator interface;
    char* start;
    char* next;
    char* previous;
    size_t size;
    uint32_t flags;
} fw64BumpAllocator;

#ifdef __cplusplus
extern "C" {
#endif

void fw64_bump_allocator_init(fw64BumpAllocator* bump, size_t size);
void fw64_bump_allocator_init_from_buffer(fw64BumpAllocator* bump, char* buffer, size_t size);
void fw64_bump_allocator_reset(fw64BumpAllocator* bump);
void fw64_bump_allocator_uninit(fw64BumpAllocator* bump);

void* fw64_bump_allocator_malloc(fw64BumpAllocator* allocator, size_t size);
void* fw64_bump_allocator_memalign(fw64BumpAllocator* allocator, size_t align, size_t size);
void fw64_bump_allocator_free(fw64BumpAllocator* allocator, void* ptr);

/** Note: this implementation will call \ref fw64_bump_allocator_free on ptr and return NULL if size is 0. */
void* fw64_bump_allocator_realloc(fw64BumpAllocator* allocator, void* ptr, size_t size);

uint32_t fw64_bump_allocator_committed(fw64BumpAllocator* bump);

#ifdef __cplusplus
}
#endif
