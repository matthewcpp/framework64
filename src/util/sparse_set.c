#include "framework64/util/sparse_set.h"

#include <stdlib.h>
#include <string.h>

/** Maps a sparse array index to a dense array index.
 * dense_index is updated when the item it refers to is swapped to a new index as part of a deletion.
 * free_list_next contains the index of the sparse array of the next hole to be used when allocating.
 */
struct fw64LookupInfo{
    uint16_t dense_index;
    uint16_t free_list_next;
};

typedef struct {
    uint16_t lookup_index;
    uint16_t generation;
} fw64SparseSetItemMetadata;

#define FW64_SPARSE_SET_DEFAULT_CAP 8
#define FW64_SPARSE_SET_ITEM_OVERHEAD sizeof(fw64SparseSetItemMetadata)
#define FW64_SPARSE_SET_INVALID_INDEX USHRT_MAX

#define FW64_SPARSE_SET_GET_DENSE_ARR_ITEM(sparse_set, index) \
    ((sparse_set)->dense_arr + ((index) * (sparse_set)->item_size))

#define FW64_SPARSE_SET_GET_DENSE_ARR_METADATA(sparse_set, index) \
    ((fw64SparseSetItemMetadata*)FW64_SPARSE_SET_GET_DENSE_ARR_ITEM((sparse_set), (index)))

#define FW64_SPARSE_SET_GET_DENSE_ARR_DATA(sparse_set, index) \
    (FW64_SPARSE_SET_GET_DENSE_ARR_ITEM((sparse_set), (index)) + FW64_SPARSE_SET_ITEM_OVERHEAD)

void fw64_sparse_set_init(fw64SparseSet* sparse_set, uint16_t item_size, fw64Allocator* allocator) {
    fw64_sparse_set_init_with_capacity(sparse_set, item_size, FW64_SPARSE_SET_DEFAULT_CAP, allocator);
}

void fw64_sparse_set_init_with_capacity(fw64SparseSet* sparse_set, uint16_t item_size, uint16_t item_capacity, fw64Allocator* allocator) {
    sparse_set->allocator = allocator;
    sparse_set->item_size = item_size + FW64_SPARSE_SET_ITEM_OVERHEAD;
    sparse_set->item_count = 0;

    sparse_set->arr_cap = item_capacity;
    sparse_set->dense_arr = allocator->malloc(allocator, sparse_set->arr_cap * sparse_set->item_size);
    memset(sparse_set->dense_arr, 0, sparse_set->arr_cap * sparse_set->item_size);

    sparse_set->lookup_arr = allocator->malloc(allocator, sparse_set->arr_cap * sizeof (fw64LookupInfo));
    sparse_set->free_list = FW64_SPARSE_SET_INVALID_INDEX;
}

void fw64_sparse_set_uninit(fw64SparseSet* sparse_set) {
    sparse_set->allocator->free(sparse_set->allocator, sparse_set->dense_arr);
    sparse_set->allocator->free(sparse_set->allocator, sparse_set->lookup_arr);
}

/**
 * Determines the index in the lookup array to use when creating a new item.
 * If non empty, it will pop and return the first value in the free list.
 */
static uint16_t fw64_sparse_set_next_lookup_arr_index(fw64SparseSet* sparse_set) {
    if (sparse_set->free_list == FW64_SPARSE_SET_INVALID_INDEX)
        return sparse_set->item_count;

    uint16_t lookup_index = sparse_set->free_list;

    fw64LookupInfo* lookup_info = sparse_set->lookup_arr + sparse_set->free_list;
    sparse_set->free_list = lookup_info->free_list_next;
    lookup_info->free_list_next = FW64_SPARSE_SET_INVALID_INDEX;

    return lookup_index;
}

static void fw64_sparse_set_push_free_list_index(fw64SparseSet* sparse_set, uint16_t index) {
    fw64LookupInfo* free_list_item = sparse_set->lookup_arr + index;

    free_list_item->free_list_next = sparse_set->free_list;
    free_list_item->dense_index = FW64_SPARSE_SET_INVALID_INDEX;

    sparse_set->free_list = index;
}

static void fw64_sparse_set_grow_arrays(fw64SparseSet* sparse_set) {
    sparse_set->arr_cap *= 2;
    sparse_set->dense_arr = sparse_set->allocator->realloc(sparse_set->allocator, sparse_set->dense_arr, sparse_set->arr_cap * sparse_set->item_size);
    sparse_set->lookup_arr = sparse_set->allocator->realloc(sparse_set->allocator, sparse_set->lookup_arr, sparse_set->arr_cap * sizeof (fw64LookupInfo));

    fw64SparseSetItemMetadata* new_item = FW64_SPARSE_SET_GET_DENSE_ARR_METADATA(sparse_set, sparse_set->item_count);
    memset(new_item, 0, sparse_set->arr_cap / 2 * sparse_set->item_size);
}

void* fw64_sparse_set_alloc(fw64SparseSet* sparse_set, fw64SparseSetHandle* handle) {
    if (sparse_set->item_count == sparse_set->arr_cap)
        fw64_sparse_set_grow_arrays(sparse_set);

    uint16_t dense_index = sparse_set->item_count;
    uint16_t lookup_index = fw64_sparse_set_next_lookup_arr_index(sparse_set);

    fw64SparseSetItemMetadata* dense_handle = FW64_SPARSE_SET_GET_DENSE_ARR_METADATA(sparse_set, dense_index);
    dense_handle->lookup_index = lookup_index;

    fw64LookupInfo* lookup_info = sparse_set->lookup_arr + lookup_index;
    lookup_info->dense_index = dense_index;

    sparse_set->item_count += 1;

    if (handle) {
        handle->lookup_index = lookup_index;
        handle->generation = dense_handle->generation;
    }

    return FW64_SPARSE_SET_GET_DENSE_ARR_DATA(sparse_set, dense_index);
}

int fw64_sparse_set_handle_is_valid(fw64SparseSet* sparse_set, fw64SparseSetHandle handle) {
    fw64LookupInfo* lookup_info = sparse_set->lookup_arr + handle.lookup_index;

    if (lookup_info->dense_index == FW64_SPARSE_SET_INVALID_INDEX)
        return 0;

    fw64SparseSetItemMetadata* dense_item = FW64_SPARSE_SET_GET_DENSE_ARR_METADATA(sparse_set, lookup_info->dense_index);

    if (dense_item->generation != handle.generation)
        return 0;

    return 1;
}

int fw64_sparse_set_delete(fw64SparseSet* sparse_set, fw64SparseSetHandle handle) {
    if (!fw64_sparse_set_handle_is_valid(sparse_set, handle))
        return 0;

    // get the lookup and corresponding dense info for the item that is to be deleted and the last item in the array
    fw64LookupInfo* target_lookup_info = sparse_set->lookup_arr + handle.lookup_index;
    fw64SparseSetItemMetadata* target_item = FW64_SPARSE_SET_GET_DENSE_ARR_METADATA(sparse_set, target_lookup_info->dense_index);

    fw64SparseSetItemMetadata* last_item = FW64_SPARSE_SET_GET_DENSE_ARR_METADATA(sparse_set, sparse_set->item_count - 1);
    fw64LookupInfo* last_item_lookup_info = sparse_set->lookup_arr + last_item->lookup_index;

    if (target_item != last_item) {
        // copy the last item into the slot of the item that is being deleted.
        memcpy(target_item, last_item, sparse_set->item_size);

        // update the links for the last item's new position
        target_item->lookup_index = last_item->lookup_index;
        last_item_lookup_info->dense_index = target_lookup_info->dense_index;
    }

    fw64_sparse_set_push_free_list_index(sparse_set, handle.lookup_index);

    last_item->generation += 1;
    sparse_set->item_count -= 1;

    return 1;
}

void* fw64_sparse_set_get_item(fw64SparseSet* sparse_set, fw64SparseSetHandle handle) {
    if (!fw64_sparse_set_handle_is_valid(sparse_set, handle))
        return NULL;

    return FW64_SPARSE_SET_GET_DENSE_ARR_DATA(sparse_set, sparse_set->lookup_arr[handle.lookup_index].dense_index);
}

void fw64_sparse_set_clear(fw64SparseSet* sparse_set) {
    for (uint16_t i = 0; i < sparse_set->item_count; i++) {
        fw64SparseSetItemMetadata* target_item = FW64_SPARSE_SET_GET_DENSE_ARR_METADATA(sparse_set, i);
        target_item->generation += 1;
    }

    sparse_set->item_count = 0;
    sparse_set->free_list = FW64_SPARSE_SET_INVALID_INDEX;
}

void fw64_sparse_set_itr_init(fw64SparseSetItr* itr, fw64SparseSet* sparse_set) {
    itr->sparse_set = sparse_set;
    itr->index = 0;
}

int fw64_sparse_set_itr_has_next(fw64SparseSetItr* itr) {
    return itr->index < itr->sparse_set->item_count;
}

void fw64_sparse_set_itr_next(fw64SparseSetItr* itr) {
    itr->index += 1;
}

void* fw64_sparse_set_itr_get_item(fw64SparseSetItr* itr) {
    return FW64_SPARSE_SET_GET_DENSE_ARR_DATA(itr->sparse_set, itr->index);
}
