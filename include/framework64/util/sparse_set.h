#pragma once

/** \file sparse_set.h */

#include "framework64/allocator.h"

#include <limits.h>
#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct fw64LookupInfo fw64LookupInfo;

/** This structure represents a handle contained in the sparse set.
 * Do not store raw pointers to items returned from \ref fw64_sparse_set_alloc or \ref fw64_sparse_set_get_item
 * Instead store this handle and use it to retrieve a pointer to the corresponding item.
 */
typedef struct {
    uint16_t lookup_index;
    uint16_t generation;
} fw64SparseSetHandle;

/**
 * A sparse set is useful for data of game objects and track their lifecycle.
 * There is a dense array which is used for efficient iteration over the whole set.
 * A corresponding sparse array is used to track items as they move in the dense array.
 * The arrays will dynamically grow as more items are added into the set.
 * As items are added and deleted, the dense array is updated and an objects location in memory may change.
 * For this reason, use a \ref fw64SparseSetHandle as a persistent key for a particular object.
 */
typedef struct {
    uint16_t item_size;
    uint16_t item_count;
    uint16_t arr_cap;
    uint16_t free_list;

    char* dense_arr;
    fw64LookupInfo* lookup_arr;
    fw64Allocator* allocator;
} fw64SparseSet;


/**
 * Initializes a new, empty sparse set.
 * @param item_size the size, in bytes of the items that will be stored in the set.
 * @param allocator allocator which will be used for all allocations by this data structure
 */
void fw64_sparse_set_init(fw64SparseSet* sparse_set, uint16_t item_size, fw64Allocator* allocator);

/**
 * Initializes a new, empty sparse set.
 * @param item_size the size, in bytes of the items that will be stored in the set.
 * @param item_capacity the number of items that the data structure should be set up to hold
 * @param allocator allocator which will be used for all allocations by this data structure
 */
void fw64_sparse_set_init_with_capacity(fw64SparseSet* sparse_set, uint16_t item_size, uint16_t item_capacity, fw64Allocator* allocator);

/**
 * Frees all resources used by the sparse set.
 */
void fw64_sparse_set_uninit(fw64SparseSet* sparse_set);

/**
 * Allocates a new object in the set.
 * @param pointer which will receive the handle for this new item.
 * @param handle pointer which will received handle data to access this item in future calls.
 * @return pointer to new object.  Be sure to cast this pointer to that of the actual struct contained in the set and initialize it.
 */
void* fw64_sparse_set_alloc(fw64SparseSet* sparse_set, fw64SparseSetHandle* handle);

/**
 * Gets an item out of the set.
 * @param handle handle returned from call to \ref fw64_sparse_set_alloc
 * @return pointer to item, or NULL if the handle was invalid.
 */
void* fw64_sparse_set_get_item(fw64SparseSet* sparse_set, fw64SparseSetHandle handle);

/**
 * Removes an item from the set.
 * @param handle handle returned from call to \ref fw64_sparse_set_alloc
 * @return non zero value if the handle was valid and an item was deleted, otherwise zero.
 */
int fw64_sparse_set_delete(fw64SparseSet* sparse_set, fw64SparseSetHandle handle);

/**
 * Checks if a handle is valid
 * @param handle handle returned from call to \ref fw64_sparse_set_alloc
 * @return non zero value if the handle is valid, otherwise zero
 */
int fw64_sparse_set_handle_is_valid(fw64SparseSet* sparse_set, fw64SparseSetHandle handle);


/**
 * Clears the set and invalidates all handles.
 */
void fw64_sparse_set_clear(fw64SparseSet* sparse_set);

#define fw64_sparse_set_get_allocator(sparse_set) ((sparse_set)->allocator)

/**
 * Structure used to iterate over all items in a sparse set.
 * Note that any insertion, deletion, or clear operation will invalidate an iterator.
 */
typedef struct {
    fw64SparseSet* sparse_set;
    uint16_t index;
} fw64SparseSetItr;

/**
 * Initializes a sparse set iterator.  Sets the the iterator to the first item in the set
 */
void fw64_sparse_set_itr_init(fw64SparseSetItr* itr, fw64SparseSet* sparse_set);

/**
 * Gets whether there are additional items to iterate over.
 * @return Non zero value if there are more items to iterate over
 */
int fw64_sparse_set_itr_has_next(fw64SparseSetItr* itr);

/**
 * Advances the iterator to the next item.
 */
void fw64_sparse_set_itr_next(fw64SparseSetItr* itr);

/**
 * Gets the current item
 * @return pointer to the current item.
 */
void* fw64_sparse_set_itr_get_item(fw64SparseSetItr* itr);

#ifdef __cplusplus
}
#endif

