#pragma once

#include "framework64/sprite_batch.h"

typedef struct {

} N64SpritebatchTextLayer;

typedef struct {

} N64SpritebatchSpriteLayer;

typedef struct {

} N64SpritebatchRectLayer;

typedef union {
    N64SpritebatchTextLayer text;
    N64SpritebatchSpriteLayer sprite;
    N64SpritebatchRectLayer rect;
} N64SpriteBatchLayerContainer;

typedef struct {
    fw64SpriteBatchLayerType type;
    N64SpriteBatchLayerContainer handle;
} N64SpriteBatchLayer;


struct fw64SpriteBatch {
    N64SpriteBatchLayer* layers;
    size_t layer_count;
    N64SpriteBatchLayer* active_layer;
    fw64Allocator* allocator;
};
