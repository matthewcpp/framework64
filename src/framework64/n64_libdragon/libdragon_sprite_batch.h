#pragma once

#include "framework64/sprite_batch.h"
#include "framework64/vec2.h"

#include "framework64/util/dynamic_vector.h"
#include "framework64/static_vector.h"

#include <libdragon.h>

typedef struct {
    Vec2 pos;
    fw64Texture* texture;
} SpriteBlitInfo;

typedef struct fw64SpritebatchChunk fw64SpritebatchChunk;
typedef struct fw64SpriteBatchTexture fw64SpriteBatchTexture;

typedef struct {
    fw64Texture* texture;
    uint32_t index;
} fw64SpriteBatchTexInfo;

typedef union {
    fw64SpriteBatchTexInfo tex_info;
    uint64_t hash;
} fw64SpriteBatchTexureId;

/** A chunk of verties to draw for a particular texture.
 * These chunks are pooled by their static vector size.
 */
struct fw64SpritebatchChunk {
    fw64StaticVector vertices;
    fw64SpritebatchChunk* next;
    int id;
};

/** Structure for drawing all instances of a single texture / frame combo.
 * These objects are pooled
 */
struct fw64SpriteBatchTexture {
    fw64SpriteBatchTexureId id;
    fw64SpritebatchChunk* first;
    fw64SpritebatchChunk* last;
    fw64SpriteBatchTexture* next;
};

typedef struct  {
    fw64DynamicVector batches;
} fw64SpriteBatchLayer;

struct fw64SpriteBatch {
    fw64Allocator* allocator;
    fw64SpriteBatchLayer* layers;
    fw64SpriteBatchTexture* texture_pool;
    // TODO: Partition by static size
    fw64SpritebatchChunk* chunk_pool;
    size_t active_layer;
    size_t layer_count;
    fw64ColorRGBA8 color;
    int chunk_id;
};