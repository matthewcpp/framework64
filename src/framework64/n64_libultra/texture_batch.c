#include "framework64/n64_libultra/texture_batch.h"

#include "framework64/n64_libultra/display_list.h"

#include <string.h>

static void return_batch_to_pool(fw64N64TextureBatch* texture_batch, fw64N64QuadBatch* quad_batch);

void fw64_n64_texture_batch_init(fw64N64TextureBatch* texture_batch, size_t layer_count, fw64Allocator* allocator) {
    texture_batch->allocator = allocator;
    texture_batch->layer_count = layer_count;

    texture_batch->layers = allocator->malloc(allocator, sizeof(fw64TextureBatchLayer) * texture_batch->layer_count);
    memset(texture_batch->layers, 0, sizeof(fw64TextureBatchLayer) * texture_batch->layer_count);
    memset(texture_batch->pool, 0, sizeof(fw64N64QuadBatch*) * TEXT_BATCH_POOL_BUCKET_COUNT);

    texture_batch->active_layer = texture_batch->layers;
}

void fw64_n64_texture_batch_uninit(fw64N64TextureBatch* texture_batch) {
    fw64Allocator* allocator = texture_batch->allocator;

    for (size_t l = 0; l < texture_batch->layer_count; l++) {
        fw64TextureBatchLayer* layer = texture_batch->layers + l;

        for (size_t i = 0; i <  layer->batch_count; i++) {
            fw64_n64_quad_batch_uninit(layer->batches[i], allocator);
        }

        allocator->free(allocator, layer->batches);
        allocator->free(allocator, layer);
    }

    for (size_t i = 0; i < TEXT_BATCH_POOL_BUCKET_COUNT; i++) {
        fw64N64QuadBatch* current = texture_batch->pool[i];

        while (current) {
            fw64N64QuadBatch* next = current->next;
            fw64_n64_quad_batch_uninit(current, allocator);
            current = next;
        }
    }

    allocator->free(allocator, texture_batch);
}

void fw64_n64_quad_batch_uninit(fw64N64QuadBatch* quad_batch, fw64Allocator* allocator) {
    allocator->free(allocator, quad_batch->vertices);

    if (quad_batch->display_list) {
        allocator->free(allocator, quad_batch->display_list);
    }

    allocator->free(allocator, quad_batch);
}

int fw64_n64_texture_batch_set_active_layer(fw64N64TextureBatch* batch, size_t index) {
    if (index >= batch->layer_count) {
        return 0;
    }

    batch->active_layer = batch->layers + index;

    return 1;
}

#define batch_pool_index(capacity) (((capacity) / FW64_N64_CHAR_BATCH_GROWTH_AMOUNT) - 1)

static fw64N64QuadBatch* get_pooled_char_batch(fw64N64TextureBatch* texture_batch, uint16_t capacity) {
    size_t pool_index = batch_pool_index(capacity);

    if (texture_batch->pool[pool_index]) {
        fw64N64QuadBatch* char_batch = texture_batch->pool[pool_index];
        texture_batch->pool[pool_index] = char_batch->next;
        char_batch->next = NULL;

        return char_batch;
    } else {
        return NULL;
    }
}

void return_batch_to_pool(fw64N64TextureBatch* texture_batch, fw64N64QuadBatch* quad_batch) {
    size_t pool_index = batch_pool_index(quad_batch->capacity);

    quad_batch->next = texture_batch->pool[pool_index];
    texture_batch->pool[pool_index] = quad_batch;
}

void fw64_n64_texture_batch_reset(fw64N64TextureBatch* texture_batch) {
    for (size_t l = 0; l < texture_batch->layer_count; l++) {
        fw64TextureBatchLayer* layer = texture_batch->layers + l;

        for (uint32_t i = 0; i < layer->batch_count; i++) {
            return_batch_to_pool(texture_batch, layer->batches[i]);
        }

        layer->batch_count = 0;
    }
}

#define INVALID_BATCH_INDEX UINT32_MAX

static uint32_t find_batch_index_rec(fw64N64TextureBatch* texture_batch, int min_index, int max_index, fw64N64QuadBatchId id) {
    uint32_t center_index = min_index + (max_index - min_index) / 2;
    if (min_index > max_index) return INVALID_BATCH_INDEX;

    fw64N64QuadBatch* center_batch = texture_batch->active_layer->batches[center_index];

    if (center_batch->id.hash == id.hash) {
        return center_index;
    }
    else if (center_batch->id.hash > id.hash) {
        return find_batch_index_rec(texture_batch, min_index, center_index - 1, id);
    }
    else if (center_batch->id.hash < id.hash) {
        return find_batch_index_rec(texture_batch, center_index + 1, max_index, id);
    }
    else {
        return center_index;
    }
}

// https://rosettacode.org/wiki/Sorting_algorithms/Insertion_sort#C
static void sort_batches(fw64N64QuadBatch **a, const size_t n) {
    for(size_t i = 1; i < n; ++i) {
        fw64N64QuadBatch* key = a[i];
        size_t j = i;
        while( (j > 0) && (key->id.hash < a[j - 1]->id.hash) ) {
            a[j] = a[j - 1];
            --j;
        }
        a[j] = key;
    }
}

static fw64N64QuadBatch* create_new_char_batch(fw64N64TextureBatch* texture_batch, uint16_t capacity) {
    fw64N64QuadBatch* char_batch = texture_batch->allocator->malloc(texture_batch->allocator, sizeof(fw64N64QuadBatch));
    memset(char_batch, 0, sizeof(fw64N64QuadBatch));

    char_batch->vertices = texture_batch->allocator->memalign(texture_batch->allocator, 8, sizeof(Vtx) * 4 * capacity);
    char_batch->capacity = capacity;

    return char_batch;
}

static fw64N64QuadBatch* get_or_create_new_char_batch(fw64N64TextureBatch* texture_batch, fw64N64QuadBatchId id, uint16_t capacity) {
    fw64N64QuadBatch* char_batch = get_pooled_char_batch(texture_batch, capacity);
    fw64TextureBatchLayer* layer = texture_batch->active_layer;

    if (!char_batch) {
        char_batch = create_new_char_batch(texture_batch, capacity);
    }

    char_batch->count = 0;
    char_batch->id = id;
    if (layer->batch_count == layer->batch_capacity) {
        layer->batch_capacity = layer->batch_capacity == 0 ? 8 : layer->batch_capacity * 2;
        fw64N64QuadBatch** old_batches = layer->batches;

        layer->batches = texture_batch->allocator->malloc(texture_batch->allocator, sizeof(fw64N64QuadBatch*) * layer->batch_capacity);
        memcpy(layer->batches, old_batches, layer->batch_count * sizeof(fw64N64QuadBatch*));
        texture_batch->allocator->free(texture_batch->allocator, old_batches);
    }

    layer->batches[layer->batch_count++] = char_batch;
    sort_batches(layer->batches, layer->batch_count);

    return char_batch;
}

static fw64N64QuadBatch* grow_char_batch(fw64N64TextureBatch* texture_batch, uint32_t batch_index) {
    fw64N64QuadBatch* existing_batch = texture_batch->active_layer->batches[batch_index];
    uint16_t new_capacity = existing_batch->capacity + FW64_N64_CHAR_BATCH_GROWTH_AMOUNT;
    fw64N64QuadBatch* new_batch = get_pooled_char_batch(texture_batch, new_capacity);

    if (!new_batch) {
        new_batch = create_new_char_batch(texture_batch, new_capacity);
    }

    new_batch->id = existing_batch->id;
    new_batch->count = existing_batch->count;
    memcpy(new_batch->vertices, existing_batch->vertices, existing_batch->count * 4 * sizeof(Vtx));

    texture_batch->active_layer->batches[batch_index] = new_batch;
    return_batch_to_pool(texture_batch, existing_batch);

    return new_batch;
}

fw64N64QuadBatch* fw64_n64_texture_batch_get_batch(fw64N64TextureBatch* texture_batch, fw64Texture* texture, uint32_t frame) {
    fw64N64QuadBatchId id;
    id.tex_info.texture = texture;
    id.tex_info.index = frame;

    if (texture_batch->active_layer->batch_count == 0) {
        return get_or_create_new_char_batch(texture_batch, id, FW64_N64_CHAR_BATCH_GROWTH_AMOUNT);
    }

    uint32_t batch_index = find_batch_index_rec(texture_batch, 0, texture_batch->active_layer->batch_count - 1, id);

    if (batch_index == INVALID_BATCH_INDEX) {
        return get_or_create_new_char_batch(texture_batch, id, FW64_N64_CHAR_BATCH_GROWTH_AMOUNT);
    }

    fw64N64QuadBatch* char_batch = texture_batch->active_layer->batches[batch_index];

    if (fw64_n64_char_batch_is_full(char_batch)) {
        char_batch = grow_char_batch(texture_batch, batch_index);
    }

    return char_batch;
    
}

#define TEXTURE_LOAD_GFX_SIZE 16

void fw64_n64_texture_batch_finalize(fw64N64TextureBatch* texture_batch) {
    fw64TextureState texture_state;
    fw64_texture_state_default(&texture_state);

    for (size_t l = 0; l < texture_batch->layer_count; l++) {
        fw64TextureBatchLayer* layer = texture_batch->layers + l;

        for (size_t i = 0; i < layer->batch_count; i++) {
            fw64N64QuadBatch* batch = layer->batches[i];

            if (batch->display_list == NULL) {
                size_t display_list_size = TEXTURE_LOAD_GFX_SIZE + fw64_n64_compute_quad_display_list_size(batch->capacity);
                batch->display_list = texture_batch->allocator->memalign(texture_batch->allocator, 8, sizeof(Gfx) * display_list_size);
            }

            Gfx* display_list = fw64_n64_load_texture(&texture_state, batch->display_list, batch->id.tex_info.texture, batch->id.tex_info.index);
            display_list = fw64_n64_create_quad_display_list(display_list, batch->vertices, batch->count);
            gSPEndDisplayList(display_list++);
        }
    }
}