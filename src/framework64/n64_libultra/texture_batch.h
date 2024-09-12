#pragma once

#include "framework64/allocator.h"
#include "framework64/n64_libultra/texture.h"

#include <nusys.h>

#include <stddef.h>
#include <stdint.h>

typedef struct fw64N64QuadBatch fw64N64QuadBatch;

typedef struct {
    fw64Texture* texture;
    uint32_t index;
} fw64N64TextureBatchTexInfo;

typedef union {
    fw64N64TextureBatchTexInfo tex_info;
    uint64_t hash;
} fw64N64QuadBatchId;

struct fw64N64QuadBatch {
    fw64N64QuadBatchId id;
    Vtx* vertices;
    Gfx* display_list;

    /** The number of quad instances currently batched*/
    uint16_t count;

    /** The number of quad instances this bacth can accomodate*/
    uint16_t capacity;

    /** Intrusive list pointer for pooling */
    fw64N64QuadBatch* next;
};

void fw64_n64_quad_batch_uninit(fw64N64QuadBatch* quad_batch, fw64Allocator* allocator);

#define fw64_n64_char_batch_is_full(char_batch) ((char_batch)->count == (char_batch)->capacity)

typedef struct {
    fw64N64QuadBatch** batches;
    uint16_t batch_count;
    uint16_t batch_capacity;
} fw64TextureBatchLayer;

#define TEXT_BATCH_POOL_BUCKET_COUNT 8
#define FW64_N64_CHAR_BATCH_GROWTH_AMOUNT 4

typedef struct {
    fw64Allocator* allocator;
    fw64TextureBatchLayer* layers;
    fw64TextureBatchLayer* active_layer;
    size_t layer_count;
    fw64N64QuadBatch* pool[TEXT_BATCH_POOL_BUCKET_COUNT];
} fw64N64TextureBatch;

void fw64_n64_texture_batch_init(fw64N64TextureBatch* batch, size_t layer_count, fw64Allocator* allocator);
void fw64_n64_texture_batch_uninit(fw64N64TextureBatch* batch);
void fw64_n64_texture_batch_reset(fw64N64TextureBatch* batch);
fw64N64QuadBatch* fw64_n64_texture_batch_get_batch(fw64N64TextureBatch* batch, fw64Texture* texture, uint32_t frame);

int fw64_n64_texture_batch_set_active_layer(fw64N64TextureBatch* batch, size_t index);

void fw64_n64_texture_batch_finalize(fw64N64TextureBatch* batch);
