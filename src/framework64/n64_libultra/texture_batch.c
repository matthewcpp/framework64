#include "framework64/n64_libultra/texture_batch.h"

#include "framework64/n64_libultra/display_list.h"

#include <string.h>

#define DEFAULT_LAYER_BATCHED_TEXTURE_CAPACITY 8

void fw64_n64_texture_batch_init(fw64N64TextureBatch* texture_batch, size_t layer_count, fw64Allocator* allocator) {
    texture_batch->allocator = allocator;
    texture_batch->layer_count = layer_count;
    texture_batch->layers = fw64_allocator_malloc(allocator, sizeof(fw64TextureBatchLayer) * texture_batch->layer_count);

    // initialize layers
    for (size_t i = 0; i < layer_count; i++) {
        fw64TextureBatchLayer* layer = texture_batch->layers + i;
        layer->batch_count = 0;
        layer->batch_capacity = DEFAULT_LAYER_BATCHED_TEXTURE_CAPACITY;
        layer->batches = fw64_allocator_malloc(allocator, layer->batch_capacity * sizeof(fw64N64BatchedTexture*));
    }

    // initalize empty pools
    texture_batch->bactched_texture_pool = NULL;
    memset(texture_batch->vertex_chunk_pool, 0, sizeof(fw64N64BatchedVertexChunk*) * TEX_BATCH_VERTEX_CHUNK_POOL_BUCKET_COUNT);

    texture_batch->active_layer = texture_batch->layers;
}

static void fw64_n64_texture_batch_delete_vertex_chunk(fw64N64BatchedVertexChunk* chunk, fw64Allocator* allocator) {
    fw64_allocator_free(allocator, chunk->display_list);
    fw64_allocator_free(allocator, chunk->vertices);
    fw64_allocator_free(allocator, chunk);
}

static void fw64_n64_texture_batch_delete_batched_texture(fw64N64BatchedTexture* batched_texture, fw64Allocator* allocator) {
    fw64N64BatchedVertexChunk* chunk = batched_texture->first_chunk;

    while (chunk) {
        fw64N64BatchedVertexChunk* chunk_to_delete = chunk;
        chunk = chunk->next;
        fw64_n64_texture_batch_delete_vertex_chunk(chunk_to_delete, allocator);
    }

    fw64_allocator_free(allocator, batched_texture);
}

void fw64_n64_texture_batch_uninit(fw64N64TextureBatch* texture_batch) {
    fw64Allocator* allocator = texture_batch->allocator;

    for (size_t l = 0; l < texture_batch->layer_count; l++) {
        fw64TextureBatchLayer* layer = texture_batch->layers + l;

        for (size_t i = 0; i <  layer->batch_count; i++) {
            fw64_n64_texture_batch_delete_batched_texture(layer->batches[i], texture_batch->allocator);;
        }

        fw64_allocator_free(allocator, layer->batches);
        fw64_allocator_free(allocator, layer);
    }

    for (size_t i = 0; i < TEX_BATCH_VERTEX_CHUNK_POOL_BUCKET_COUNT; i++) {
        fw64N64BatchedVertexChunk* pooled_chunk = texture_batch->vertex_chunk_pool[i];

        while (pooled_chunk) {
            fw64N64BatchedVertexChunk* chunk_to_delete = pooled_chunk;
            pooled_chunk = pooled_chunk->next;
            fw64_n64_texture_batch_delete_vertex_chunk(chunk_to_delete, texture_batch->allocator);
        }
    }

    fw64N64BatchedTexture* pooled_texture = texture_batch->bactched_texture_pool;
    while (pooled_texture) {
        fw64N64BatchedTexture* texture_to_delete = pooled_texture;
        pooled_texture = pooled_texture->next;
        fw64_n64_texture_batch_delete_batched_texture(texture_to_delete, allocator);
    }

    fw64_allocator_free(allocator, texture_batch);
}

#define batch_pool_index(capacity) (((capacity) / TEX_BATCH_VERTEX_CHUNK_GROWTH_AMOUNT) - 1)

static void fw64_n64_texture_batch_return_chunk_to_pool(fw64N64TextureBatch* texture_batch, fw64N64BatchedVertexChunk* chunk_to_return) {
    size_t pool_index = batch_pool_index(chunk_to_return->capacity);
    
    chunk_to_return->next = texture_batch->vertex_chunk_pool[pool_index];
    texture_batch->vertex_chunk_pool[pool_index] = chunk_to_return;
}

void fw64_n64_texture_batch_reset(fw64N64TextureBatch* texture_batch) {
    for (size_t l = 0; l < texture_batch->layer_count; l++) {
        fw64TextureBatchLayer* layer = texture_batch->layers + l;

        for (uint32_t i = 0; i < layer->batch_count; i++) {
            fw64N64BatchedTexture* batched_texture = layer->batches[i];

            // return all the chunks in this batch to the pool
            // note the texture's first chunk will be NULL after this.
            // fw64_n64_texture_batch_delete_batched_texture looks at this value to see if needs to delete attached chunks
            while (batched_texture->first_chunk) {
                fw64N64BatchedVertexChunk* chunk_to_return = batched_texture->first_chunk;
                batched_texture->first_chunk = batched_texture->first_chunk->next;
                fw64_n64_texture_batch_return_chunk_to_pool(texture_batch, chunk_to_return);
            }

            batched_texture->next = texture_batch->bactched_texture_pool;
            texture_batch->bactched_texture_pool = batched_texture;
        }

        layer->batch_count = 0;
    }
}

int fw64_n64_texture_batch_set_active_layer(fw64N64TextureBatch* batch, size_t index) {
    if (index >= batch->layer_count) {
        return 0;
    }

    batch->active_layer = batch->layers + index;

    return 1;
}

#define INVALID_BATCH_INDEX UINT32_MAX

/** Attempts to locate the index of the batched texture from the active layer. */
static uint32_t find_batched_texture_rec(fw64N64TextureBatch* texture_batch, fw64TextureBatchLayer* layer, int min_index, int max_index, fw64N64BatchedTextureId id) {
    uint32_t center_index = min_index + (max_index - min_index) / 2;
    if (min_index > max_index) return INVALID_BATCH_INDEX;

    fw64N64BatchedTexture* center_batch = layer->batches[center_index];

    if (center_batch->id.hash == id.hash) {
        return center_index;
    }
    else if (center_batch->id.hash > id.hash) {
        return find_batched_texture_rec(texture_batch, layer, min_index, center_index - 1, id);
    }
    else if (center_batch->id.hash < id.hash) {
        return find_batched_texture_rec(texture_batch, layer, center_index + 1, max_index, id);
    }
    else {
        return center_index;
    }
}

// https://rosettacode.org/wiki/Sorting_algorithms/Insertion_sort#C
static void sort_batches(fw64N64BatchedTexture **a, const size_t n) {
    for(size_t i = 1; i < n; ++i) {
        fw64N64BatchedTexture* key = a[i];
        size_t j = i;
        while( (j > 0) && (key->id.hash < a[j - 1]->id.hash) ) {
            a[j] = a[j - 1];
            --j;
        }
        a[j] = key;
    }
}

#define MAX_CHUNK_CAPACITY (TEX_BATCH_VERTEX_CHUNK_POOL_BUCKET_COUNT * TEX_BATCH_VERTEX_CHUNK_GROWTH_AMOUNT)

/** First looks in the pool to see if we have a chunk that matches the capacity requirements. Otherwise will create a new one. */
static fw64N64BatchedVertexChunk* get_vertex_chunk(fw64N64TextureBatch* texture_batch, uint16_t capacity) {
    fw64N64BatchedVertexChunk* vertex_chunk = NULL;

    if (capacity > MAX_CHUNK_CAPACITY) {
        capacity = TEX_BATCH_VERTEX_CHUNK_GROWTH_AMOUNT;
    }

    size_t pool_index = batch_pool_index(capacity);
    if (texture_batch->vertex_chunk_pool[pool_index]) {
        vertex_chunk = texture_batch->vertex_chunk_pool[pool_index];
        texture_batch->vertex_chunk_pool[pool_index] = vertex_chunk->next;
    }

    if (!vertex_chunk) {
        vertex_chunk = fw64_allocator_malloc(texture_batch->allocator, sizeof(fw64N64BatchedVertexChunk));

        vertex_chunk->capacity = capacity;
        vertex_chunk->vertices = fw64_allocator_memalign(texture_batch->allocator, 8, sizeof(Vtx) * 4 * capacity);
        vertex_chunk->display_list = fw64_allocator_memalign(texture_batch->allocator, 8, sizeof(Gfx) * fw64_n64_compute_quad_display_list_size(capacity));
    }

    vertex_chunk->count = 0;
    vertex_chunk->next = NULL;
    vertex_chunk->previous = NULL;

    return vertex_chunk;
}

// This is the number of instruction required to load a texture
#define TEXTURE_LOAD_GFX_COUNT 16

// This is the acctual size of the display list: load the texture + the final gSPEndDisplayList instruction
#define BATCHED_TEXTURE_GFX_COUNT (TEXTURE_LOAD_GFX_COUNT + 1)

/** Retrieves an existing batched texture from the active layer object or creates a new one*/
static fw64N64BatchedTexture* fw64_n64_texture_batch_get_batched_texture(fw64N64TextureBatch* texture_batch, fw64Texture* texture, uint32_t frame) {
    fw64N64BatchedTextureId id;
    id.tex_info.texture = texture;
    id.tex_info.index = frame;

    fw64TextureBatchLayer* active_layer = texture_batch->active_layer;
    uint32_t batched_texture_index = find_batched_texture_rec(texture_batch, active_layer, 0, active_layer->batch_count - 1, id);

    // return existing batch from active layer
    if (batched_texture_index != INVALID_BATCH_INDEX) {
        return texture_batch->active_layer->batches[batched_texture_index];
    }

    // batch does not exist, get pooled or create new one
    fw64N64BatchedTexture* batched_texture = NULL;
    if (texture_batch->bactched_texture_pool) {
        batched_texture = texture_batch->bactched_texture_pool;
        texture_batch->bactched_texture_pool = batched_texture->next;
    }
    else {
        batched_texture = fw64_allocator_malloc(texture_batch->allocator, sizeof(fw64N64BatchedTexture));
    }

    batched_texture->id = id;
    batched_texture->first_chunk = NULL;
    batched_texture->last_chunk = NULL;
    batched_texture->next = NULL;

    // grow batch list for layer if needed
    if (active_layer->batch_count == active_layer->batch_capacity) {
        active_layer->batch_capacity *= 2;
        active_layer->batches = fw64_allocator_realloc(texture_batch->allocator, active_layer->batches, active_layer->batch_capacity * sizeof(fw64N64BatchedTexture*));
    }

    active_layer->batches[active_layer->batch_count++] = batched_texture;
    sort_batches(active_layer->batches, active_layer->batch_count);

    return batched_texture;
}

/* main method that will be called by external clients */
fw64N64BatchedVertexChunk* fw64_n64_texture_batch_get_vertex_chunk(fw64N64TextureBatch* texture_batch, fw64Texture* texture, uint32_t frame) {
    fw64N64BatchedTexture* batched_texture = fw64_n64_texture_batch_get_batched_texture(texture_batch, texture, frame);
    // check for conditions that require modifications to the vertex chunk list
    // the batch is empty
    if (!batched_texture->last_chunk) {
        fw64N64BatchedVertexChunk* chunk = get_vertex_chunk(texture_batch, TEX_BATCH_VERTEX_CHUNK_GROWTH_AMOUNT);
        chunk->previous = NULL;
        batched_texture->first_chunk = chunk;
        batched_texture->last_chunk = chunk;
    }  // the batch is full
    else if (batched_texture->last_chunk->count == batched_texture->last_chunk->capacity) {
        // note the `previous` and `next` values of new chunk is set to NULL when it is created
        fw64N64BatchedVertexChunk* new_chunk = get_vertex_chunk(texture_batch, batched_texture->last_chunk->capacity + TEX_BATCH_VERTEX_CHUNK_GROWTH_AMOUNT);

        // if the final batch is maxed out then append the new one
        if (batched_texture->last_chunk->capacity == MAX_CHUNK_CAPACITY) {
            new_chunk->previous = batched_texture->last_chunk;
            batched_texture->last_chunk->next = new_chunk;
            batched_texture->last_chunk = new_chunk;
        } 
        else {// replace the final batch and pool it 
            fw64N64BatchedVertexChunk* chunk_to_pool = batched_texture->last_chunk;
            memcpy(new_chunk->vertices, chunk_to_pool->vertices, chunk_vertex_count(chunk_to_pool) * sizeof(Vtx));
            new_chunk->count = chunk_to_pool->count;

            if (chunk_to_pool->previous) {
                chunk_to_pool->previous->next = new_chunk;
                new_chunk->previous = chunk_to_pool->previous;
            } else { // growing first item in list
                batched_texture->first_chunk = new_chunk;
            }

            batched_texture->last_chunk = new_chunk;
            fw64_n64_texture_batch_return_chunk_to_pool(texture_batch, chunk_to_pool);
        }
    }

    return batched_texture->last_chunk;

}

void fw64_n64_texture_batch_finalize(fw64N64TextureBatch* texture_batch) {
    fw64TextureState texture_state;
    fw64_texture_state_default(&texture_state);

    for (size_t l = 0; l < texture_batch->layer_count; l++) {
        fw64TextureBatchLayer* layer = texture_batch->layers + l;

        for (size_t i = 0; i < layer->batch_count; i++) {
            fw64N64BatchedTexture* batch = layer->batches[i];

            // for each chunk create the drawing display list
            fw64N64BatchedVertexChunk* chunk = batch->first_chunk;
            while (chunk) {
                Gfx* display_list = fw64_n64_create_quad_display_list(chunk->display_list, chunk->vertices, chunk->count);
                gSPEndDisplayList(display_list++);
                chunk = chunk->next;
            }
        }
    }
}