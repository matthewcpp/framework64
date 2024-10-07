#pragma once

#include "framework64/allocator.h"
#include "framework64/n64_libultra/texture.h"

#include <nusys.h>

#include <stddef.h>
#include <stdint.h>

typedef struct fw64N64BatchedVertexChunk fw64N64BatchedVertexChunk;
typedef struct fw64N64BatchedTexture fw64N64BatchedTexture;

typedef struct {
    fw64Texture* texture;
    uint32_t index;
} fw64N64TextureBatchTexInfo;

typedef union {
    fw64N64TextureBatchTexInfo tex_info;
    uint64_t hash;
} fw64N64BatchedTextureId;

struct fw64N64BatchedVertexChunk {
    Vtx* vertices;
    Gfx* display_list;

    /** The number of quad instances currently batched*/
    uint16_t count;

    /** The number of quad instances this batch can accomodate*/
    uint16_t capacity;

    /** Intrusive list pointer for pooling and linking */
    fw64N64BatchedVertexChunk* next;
    fw64N64BatchedVertexChunk* previous;
};

#define chunk_vertex_count(chunk) ((chunk)->count * 4)

/** This struct represents a single texture that will be drawn multiple times */
struct fw64N64BatchedTexture{
    /** hash for sorting and searching */
    fw64N64BatchedTextureId id;

    /** first chunk to render */
    fw64N64BatchedVertexChunk* first_chunk;

    /** last chunk for appending vertices */
    fw64N64BatchedVertexChunk* last_chunk;

    /** instrusive pointer for pooling */
    fw64N64BatchedTexture* next;
};

void fw64_n64_quad_batch_uninit(fw64N64BatchedVertexChunk* quad_batch, fw64Allocator* allocator);

typedef struct {
    fw64N64BatchedTexture** batches;
    uint16_t batch_count;
    uint16_t batch_capacity;
} fw64TextureBatchLayer;

#define TEX_BATCH_VERTEX_CHUNK_POOL_BUCKET_COUNT 8
#define TEX_BATCH_VERTEX_CHUNK_GROWTH_AMOUNT 4


typedef struct {
    fw64Allocator* allocator;
    fw64TextureBatchLayer* layers;
    fw64TextureBatchLayer* active_layer;
    size_t layer_count;

    /** Holds available fw64N64BatchedTexture objects which can be reused */
    fw64N64BatchedTexture* bactched_texture_pool;

    /** Array of available vertex chunks.  Each bucket holds chunks of a certain capacity */
    fw64N64BatchedVertexChunk* vertex_chunk_pool[TEX_BATCH_VERTEX_CHUNK_POOL_BUCKET_COUNT];
} fw64N64TextureBatch;

void fw64_n64_texture_batch_init(fw64N64TextureBatch* batch, size_t layer_count, fw64Allocator* allocator);
void fw64_n64_texture_batch_uninit(fw64N64TextureBatch* batch);
void fw64_n64_texture_batch_reset(fw64N64TextureBatch* batch);

/** Returns a vertex chunk that has capacity capacity for at least 1 quad. */
fw64N64BatchedVertexChunk* fw64_n64_texture_batch_get_vertex_chunk(fw64N64TextureBatch* batch, fw64Texture* texture, uint32_t frame);

int fw64_n64_texture_batch_set_active_layer(fw64N64TextureBatch* batch, size_t index);

void fw64_n64_texture_batch_finalize(fw64N64TextureBatch* batch);
