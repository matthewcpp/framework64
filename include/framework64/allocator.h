#pragma once

/** \file allocator.h */
#include <stdlib.h>

typedef struct fw64Allocator fw64Allocator;

typedef void*(*fw64AllocatorMallocFunc)(fw64Allocator*, size_t);
typedef void*(*fw64AllocatorReallocFunc)(fw64Allocator*, void*, size_t);
typedef void*(*fw64AllocatorMemalignFunc)(fw64Allocator*, size_t, size_t);
typedef void(*fw64AllocatorFreeFunc)(fw64Allocator*, void*);

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
void fw64_default_allocator_init();

/** Returns the default allocator which simply forwards calls to the corresponding standard library functions. */
fw64Allocator* fw64_default_allocator();

#ifdef __cplusplus
}
#endif