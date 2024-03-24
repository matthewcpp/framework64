#pragma once
/** \file sprite_batch.h */

#include <framework64/font.h>

typedef struct fw64SpriteBatch fw64SpriteBatch;
typedef struct fw64SpriteBatchTextLayer fw64SpriteBatchTextLayer;
typedef struct fw64SpriteBatchSpriteLayer fw64SpriteBatchSpriteLayer;
typedef struct fw64SpriteBatchFillLayer fw64SpriteBatchFillLayer;

typedef enum {
    FW64_SPRITE_BATCH_LAYER_TYPE_UNKNOWN,
    FW64_SPRITE_BATCH_LAYER_TYPE_TEXT,
    FW64_SPRITE_BATCH_LAYER_TYPE_SPRITE,
    FW64_SPRITE_BATCH_LAYER_TYPE_FILL
} fw64SpriteBatchLayerType;

#ifdef __cplusplus
extern "C" {
#endif

/** 
 * Allocates sprite batch layers.  
 * Any previously allocated layers are invalidated and will need to be reinitalized. 
 */
void fw64_aprite_batch_allocate_layers(fw64SpriteBatch* sprite_batch, int layer_count);

/** Returns the type of the layer at the given index. */
fw64SpriteBatchLayerType fw64_sprite_batch_get_layer_type(fw64SpriteBatch* sprite_batch, int layer_index);

/** Returns the number of layers in the sprite batch. */
int fw64_sprite_batch_get_layer_count(fw64SpriteBatch* sprite_batch);

/** Initializes the layer at the given index for text rendering with the supplied font.*/
fw64SpriteBatchTextLayer* fw64_sprite_batch_init_text_layer(fw64SpriteBatch* sprite_batch, int layer_index, fw64Font* font);

/** Returns the text layer at the given index.  If the index is out of range or is not initialized as a text layer, returns NULL. */
fw64SpriteBatchTextLayer* fw64_sprite_batch_get_text_layer(fw64SpriteBatch* sprite_batch, int layer_index);

void fw64_sprite_batch_draw_string(fw64SpriteBatchTextLayer* layer, int x, int y, const char* text);
void fw64_sprite_batch_draw_string_count(fw64SpriteBatchTextLayer* layer, int x, int y, const char* text, uint32_t count);

/** Initializes the layer at the given index for sprite rendering with the supplied textures*/
fw64SpriteBatchSpriteLayer* fw64_sprite_batch_init_sprite_layer(fw64SpriteBatch* sprite_batch, int layer_index, fw64Texture* sprites, int texture_count);

/** Returns the sprite layer at the given index.  If the index is out of range or is not initialized as a text layer, returns NULL. */
fw64SpriteBatchSpriteLayer* fw64_sprite_batch_get_sprite_layer(fw64SpriteBatch* sprite_batch, int layer_index);

#ifdef __cplusplus
}
#endif
