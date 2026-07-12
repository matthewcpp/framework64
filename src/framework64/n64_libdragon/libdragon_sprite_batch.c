#include "libdragon_sprite_batch.h"

#include "libdragon_texture.h"
#include "libdragon_vertex.h"

#include <string.h>

static uint32_t find_spritebatch_texture(fw64SpriteBatch* spritebatch, fw64SpriteBatchLayer* layer, int min_index, int max_index, fw64SpriteBatchTexureId id);
static fw64SpriteBatchTexture* get_or_create_spritebatch_texture(fw64SpriteBatch* spritebatch, fw64SpriteBatchLayer* layer, fw64SpriteBatchTexureId id);
static fw64SpritebatchChunk* get_or_create_vertex_chunk(fw64SpriteBatch* spritebatch);

#define INVALID_BATCH_INDEX UINT32_MAX

#define FW64_SPRITEBATCH_CHUNK_QUAD_COUNT 2
/** QUAD_COUNT quads per chunk. Each quad has two triangles. Each triangle has 3 vertices */
#define FW64_SPRITEBATCH_CHUNK_VERTEX_COUNT (2 * 3 * FW64_SPRITEBATCH_CHUNK_QUAD_COUNT)

fw64SpriteBatch* fw64_spritebatch_create(size_t layer_count, fw64Allocator* allocator){
    fw64SpriteBatch* spritebatch = fw64_allocator_malloc(allocator, sizeof(fw64SpriteBatch));
    spritebatch->allocator = allocator;
    spritebatch->active_layer = 0;
    spritebatch->layers = fw64_allocator_malloc(allocator, sizeof(fw64SpriteBatchLayer) * layer_count);
    spritebatch->layer_count = layer_count;
    spritebatch->texture_pool = NULL;
    spritebatch->chunk_pool = NULL;
    spritebatch->color = (fw64ColorRGBA8){255, 255, 255, 255};
    spritebatch->chunk_id = 0;

    for (size_t i = 0; i < spritebatch->layer_count; i++) {
        fw64SpriteBatchLayer* layer = spritebatch->layers + i;
        fw64_dynamic_vector_init(&layer->batches, sizeof(fw64SpriteBatchTexture*), allocator);
    }

    return spritebatch;
}

void fw64_spritebatch_delete(fw64SpriteBatch* spritebatch) {
    for (size_t i = 0; i < spritebatch->layer_count; i++) {
        fw64SpriteBatchLayer* layer = spritebatch->layers + i;
        fw64_dynamic_vector_uninit(&layer->batches);
    }

    fw64_allocator_free(spritebatch->allocator, spritebatch->layers);
    fw64_allocator_free(spritebatch->allocator, spritebatch);
}

int fw64_spritebatch_set_active_layer(fw64SpriteBatch* sprite_batch, size_t layer_index) {
    sprite_batch->active_layer = layer_index;
}

void fw64_spritebatch_begin(fw64SpriteBatch* spritebatch) {
    for (size_t l = 0; l < spritebatch->layer_count; l++) {
        fw64SpriteBatchLayer* layer = spritebatch->layers + l;

        for (size_t b = 0; b < fw64_dynamic_vector_size(&layer->batches); b++) {
            fw64SpriteBatchTexture* texture_batch = *((fw64SpriteBatchTexture**)fw64_dynamic_vector_item(&layer->batches, b));
            fw64SpritebatchChunk* current_chunk = texture_batch->first;
        
            while (current_chunk) {
                // return the vertex chunk to the pool
                fw64SpritebatchChunk* pool_front = spritebatch->chunk_pool;
                spritebatch->chunk_pool = current_chunk;
                current_chunk = current_chunk->next;
                spritebatch->chunk_pool->next = pool_front;
            }

            // returns the texture batch to the pool
            texture_batch->next = spritebatch->texture_pool;
            spritebatch->texture_pool = texture_batch;
        }

        fw64_dynamic_vector_clear(&layer->batches);
    }
}

void fw64_spritebatch_end(fw64SpriteBatch* spritebatch) {
    (void)spritebatch;
}

void fw64_spritebatch_draw_sprite_slice(fw64SpriteBatch* spritebatch, fw64Texture* texture, int frame, int x, int y) {
    fw64SpriteBatchTexureId id = {{texture, (uint32_t)frame}};
    fw64SpriteBatchLayer* layer = spritebatch->layers + spritebatch->active_layer;
    fw64LibdragonSpriteSlice* slice = texture->image->sprites + frame;

    uint32_t texture_index = INVALID_BATCH_INDEX;
    if (fw64_dynamic_vector_size(&layer->batches) > 0) {
        texture_index = find_spritebatch_texture(spritebatch, layer, 0, fw64_dynamic_vector_size(&layer->batches) - 1, id);
    }

    fw64SpriteBatchTexture* batch_texture;
    fw64SpritebatchChunk* chunk;

    if (texture_index == INVALID_BATCH_INDEX) {
        batch_texture = get_or_create_spritebatch_texture(spritebatch, layer, id);
    } else {
        batch_texture = *(fw64SpriteBatchTexture**)fw64_dynamic_vector_item(&layer->batches, texture_index);
    }

    if (batch_texture->first == NULL) {
        // this texture doesnt have any chunks yet create a new one
        chunk = get_or_create_vertex_chunk(spritebatch);
        batch_texture->first = chunk;
        batch_texture->last = chunk;
    } else if (fw64_static_vector_size(&batch_texture->last->vertices) == FW64_SPRITEBATCH_CHUNK_VERTEX_COUNT) {
        // the last chunk of this texture is full, need to append a new one
        chunk = get_or_create_vertex_chunk(spritebatch);
        batch_texture->last->next = chunk;
        batch_texture->last = chunk;
    } else {
        // existing chunk has enough room to draw this slice
        chunk = batch_texture->last;
    }

    int width = slice->sprite->width;
    int height = slice->sprite->height;

    Fw64LibdragonSpriteVertex v0 = {
        {(x + width) << FW64_LIBDRAGON_FRACTIONAL_BITS, (y + height) << FW64_LIBDRAGON_FRACTIONAL_BITS, 0},
        0,
        {1 << FW64_LIBDRAGON_FRACTIONAL_BITS, 1 << FW64_LIBDRAGON_FRACTIONAL_BITS},
        {spritebatch->color.r, spritebatch->color.g, spritebatch->color.b, spritebatch->color.a}
    };

    Fw64LibdragonSpriteVertex v1 = {
        {x << FW64_LIBDRAGON_FRACTIONAL_BITS, (y + height) << FW64_LIBDRAGON_FRACTIONAL_BITS, 0},
        0,
        {0, 1 << FW64_LIBDRAGON_FRACTIONAL_BITS},
        {spritebatch->color.r, spritebatch->color.g, spritebatch->color.b, spritebatch->color.a}
    };

    Fw64LibdragonSpriteVertex v2 = {
        {x << FW64_LIBDRAGON_FRACTIONAL_BITS, y << FW64_LIBDRAGON_FRACTIONAL_BITS, 0},
        0,
        {0, 0},
        {spritebatch->color.r, spritebatch->color.g, spritebatch->color.b, spritebatch->color.a}
    };

    Fw64LibdragonSpriteVertex v3 = {
        {(x + width) << FW64_LIBDRAGON_FRACTIONAL_BITS, y << FW64_LIBDRAGON_FRACTIONAL_BITS, 0},
        0,
        {1 << FW64_LIBDRAGON_FRACTIONAL_BITS, 0},
        {spritebatch->color.r, spritebatch->color.g, spritebatch->color.b, spritebatch->color.a}
    };

    fw64_static_vector_push_back(&chunk->vertices, &v0);
    fw64_static_vector_push_back(&chunk->vertices, &v1);
    fw64_static_vector_push_back(&chunk->vertices, &v2);

    fw64_static_vector_push_back(&chunk->vertices, &v0);
    fw64_static_vector_push_back(&chunk->vertices, &v2);
    fw64_static_vector_push_back(&chunk->vertices, &v3);
}

// https://rosettacode.org/wiki/Sorting_algorithms/Insertion_sort#C
static void sort_batches(fw64SpriteBatchTexture **a, const size_t n) {
    return;
    for(size_t i = 1; i < n; ++i) {
        fw64SpriteBatchTexture* key = a[i];
        size_t j = i;
        while( (j > 0) && (key->id.hash < a[j - 1]->id.hash) ) {
            a[j] = a[j - 1];
            --j;
        }
        a[j] = key;
    }
}

fw64SpriteBatchTexture* get_or_create_spritebatch_texture(fw64SpriteBatch* spritebatch, fw64SpriteBatchLayer* layer, fw64SpriteBatchTexureId id) {
    fw64SpriteBatchTexture* texture;

    if (spritebatch->texture_pool) {
        texture = spritebatch->texture_pool;
        spritebatch->texture_pool = texture->next;
    } else {
        texture = fw64_allocator_malloc(spritebatch->allocator, sizeof(fw64SpriteBatchTexture));
    }

    texture->id = id;
    texture->first = NULL;
    texture->last = NULL;
    texture->next = NULL;

    fw64_dynamic_vector_push_back(&layer->batches, &texture);
    sort_batches((fw64SpriteBatchTexture**)fw64_dynamic_vector_data(&layer->batches), fw64_dynamic_vector_size(&layer->batches));

    return texture;
}

fw64SpritebatchChunk* get_or_create_vertex_chunk(fw64SpriteBatch* spritebatch) {
    fw64SpritebatchChunk* chunk;
    if (spritebatch->chunk_pool == NULL) {
        chunk = fw64_allocator_malloc(spritebatch->allocator, sizeof(fw64SpritebatchChunk));
        chunk->id = spritebatch->chunk_id++;
        fw64_static_vector_init_aligned(&chunk->vertices, sizeof(Fw64LibdragonSpriteVertex), FW64_SPRITEBATCH_CHUNK_VERTEX_COUNT, spritebatch->allocator, FW64_LIBDRAGON_VERTEX_ALIGNMENT);
    } else {
        chunk = spritebatch->chunk_pool;
        spritebatch->chunk_pool = chunk->next;
        fw64_static_vector_clear(&chunk->vertices);
    }

    chunk->next = NULL;

    return chunk;
}

/** Attempts to locate the index of the batched texture from the active layer. */
uint32_t find_spritebatch_texture(fw64SpriteBatch* spritebatch, fw64SpriteBatchLayer* layer, int min_index, int max_index, fw64SpriteBatchTexureId id) {
    uint32_t center_index = min_index + (max_index - min_index) / 2;
    if (min_index > max_index){
        return INVALID_BATCH_INDEX;
    }

    fw64SpriteBatchTexture* center_batch = *(fw64SpriteBatchTexture**)fw64_dynamic_vector_item(&layer->batches, center_index);

    if (center_batch->id.hash == id.hash) {
        return center_index;
    }
    else if (center_batch->id.hash > id.hash) {
        return find_spritebatch_texture(spritebatch, layer, min_index, center_index - 1, id);
    }
    else if (center_batch->id.hash < id.hash) {
        return find_spritebatch_texture(spritebatch, layer, center_index + 1, max_index, id);
    }
    else {
        return center_index;
    }
}

void fw64_spritebatch_draw_sprite(fw64SpriteBatch* sprite_batch, fw64Texture* texture, int x, int y) {
    int x_start = x;
    int index = 0;

    // Note: this assumes that all sprite tiles are the same width / height which should be true
    int hstride = texture->image->sprites->sprite->width;
    int vstride = texture->image->sprites->sprite->height;

    for (uint16_t vslice = 0; vslice < texture->image->info.vslices; vslice++) {
        for (uint16_t hslice = 0; hslice < texture->image->info.hslices; hslice++) {
            fw64_spritebatch_draw_sprite_slice(sprite_batch, texture, index++, x, y);
            x += hstride;
        }

        x = x_start;
        y += vstride;
    }
    
}

void fw64_spritebatch_set_color(fw64SpriteBatch* sprite_batch, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    fw64_color_rgba8_set(&sprite_batch->color, r, g, b, a);
}
