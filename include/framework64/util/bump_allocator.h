#pragma once

/** \file fps_camera.h */

#include "framework64/allocator.h"

typedef struct {
    fw64Allocator interface;
    char* start;
    char* next;
    char* previous;
    size_t size;
} fw64BumpAllocator;

#ifdef __cplusplus
extern "C" {
#endif

void fw64_bump_allocator_init(fw64BumpAllocator* bump, size_t size);
void fw64_bump_allocator_reset(fw64BumpAllocator* bump);
void fw64_bump_allocator_uninit(fw64BumpAllocator* bump);

#ifdef __cplusplus
}
#endif