#pragma once

/** \file allocator.h */
#include <stdlib.h>

typedef struct fw64Allocator fw64Allocator;

typedef void*(*fw64AllocatorMallocFunc)(fw64Allocator*, size_t size);
typedef void*(*fw64AllocatorReallocFunc)(fw64Allocator*, void* ptr, size_t new_size);
typedef void*(*fw64AllocatorMemalignFunc)(fw64Allocator*, size_t alignment, size_t size);
typedef void(*fw64AllocatorFreeFunc)(fw64Allocator*, void* ptr);

struct fw64Allocator{
    fw64AllocatorMallocFunc malloc;
    fw64AllocatorReallocFunc realloc;
    fw64AllocatorMemalignFunc memalign;
    fw64AllocatorFreeFunc free;
};

#ifdef __cplusplus
extern "C" {
#endif

/** This function is automatically card when the library is initialized.  It does not need to be called in your application. */
void _fw64_default_allocator_init();

/** Returns the default allocator which simply forwards calls to the corresponding standard library functions. */
fw64Allocator* fw64_default_allocator();

void* fw64_allocator_malloc(fw64Allocator* allocator, size_t size);
void* fw64_allocator_realloc(fw64Allocator* allocator, void* ptr, size_t new_size);
void* fw64_allocator_memalign(fw64Allocator* allocator, size_t alignment, size_t size);
void  fw64_allocator_free(fw64Allocator* allocator, void* ptr);

#ifdef __cplusplus
}
#endif
