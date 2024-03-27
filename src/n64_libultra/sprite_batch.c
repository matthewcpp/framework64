#include "framework64/n64/sprite_batch.h"

#include <string.h>

fw64SpriteBatch* fw64_sprite_batch_create(fw64Allocator* allocator){
    fw64SpriteBatch* sprite_batch = allocator->malloc(allocator, sizeof(fw64SpriteBatch));
    memset(sprite_batch, 0, sizeof(fw64SpriteBatch));

    return sprite_batch;
}

void fw64_sprite_batch_delete(fw64SpriteBatch* sprite_batch) {
    sprite_batch->allocator->free(sprite_batch->allocator, sprite_batch);
}

void fw64_sprite_batch_allocate_layers(fw64SpriteBatch* sprite_batch, size_t layer_count) {
    // TODO: Clear out existing layers
    sprite_batch->layers = sprite_batch->allocator->malloc(sprite_batch->allocator, sizeof(N64SpriteBatchLayer) * layer_count);
    sprite_batch->layer_count = layer_count;
    sprite_batch->active_layer = NULL;
}

fw64SpriteBatchLayerType fw64_sprite_batch_get_layer_type(fw64SpriteBatch* sprite_batch, size_t layer_index) {
    if (layer_index >= sprite_batch->layer_count) {
        return FW64_SPRITE_BATCH_LAYER_TYPE_UNKNOWN;
    }

    return sprite_batch->layers[layer_index].type;
}

size_t fw64_sprite_batch_get_layer_count(fw64SpriteBatch* sprite_batch) {
    return sprite_batch->layer_count;
}