#pragma once
/** \file sprite_batch.h */

#include <framework64/font.h>
#include <framework64/texture.h>

typedef struct fw64SpriteBatch fw64SpriteBatch;

typedef enum {
    FW64_SPRITE_BATCH_LAYER_TYPE_UNKNOWN,
    FW64_SPRITE_BATCH_LAYER_TYPE_TEXT,
    FW64_SPRITE_BATCH_LAYER_TYPE_SPRITE,
    FW64_SPRITE_BATCH_LAYER_TYPE_RECT
} fw64SpriteBatchLayerType;

#ifdef __cplusplus
extern "C" {
#endif

fw64SpriteBatch* fw64_sprite_batch_create(fw64Allocator* allocator);
void fw64_sprite_batch_delete(fw64SpriteBatch* sprite_batch);
void fw64_sprite_batch_reset(fw64SpriteBatch* sprite_batch);

/** 
 * Allocates sprite batch layers.  
 * Any previously allocated layers are invalidated and will need to be reinitalized. 
 */
void fw64_sprite_batch_allocate_layers(fw64SpriteBatch* sprite_batch, size_t layer_count);

/** Returns the type of the layer at the given index. */
fw64SpriteBatchLayerType fw64_sprite_batch_get_layer_type(fw64SpriteBatch* sprite_batch, size_t layer_index);

/** Returns the number of layers in the sprite batch. */
size_t fw64_sprite_batch_get_layer_count(fw64SpriteBatch* sprite_batch);

/** Sets the active layer for sprite batch commands */
int fw64_sprite_batch_set_active_layer(fw64SpriteBatch* sprite_batch, size_t layer_index);

/**
 * Sets the fill color for the active layer.
 * For sprite / text final color is computed by multiplying fill * texture color
 * For filled rectangles this color is used directly.
 * Note this value is cleared in ever fall to \ref fw64_renderer_begin
 */
void fw64_sprite_batch_set_fill_color(fw64SpriteBatch* sprite_batch, uint8_t r, uint8_t g, uint8_t b, uint8_t a);

// Text Rendering

/** Initializes the layer at the given index for text rendering with the supplied font.*/
int fw64_sprite_batch_init_text_layer(fw64SpriteBatch* sprite_batch, size_t layer_index, fw64Font* font);

void fw64_sprite_batch_draw_string(fw64SpriteBatch* sprite_batch, int x, int y, const char* text);
void fw64_sprite_batch_draw_string_count(fw64SpriteBatch* sprite_batch, int x, int y, const char* text, uint32_t count);

// Sprite Rendering

/** Initializes the layer at the given index for sprite rendering with the supplied textures*/
int fw64_sprite_batch_init_sprite_layer(fw64SpriteBatch* sprite_batch, size_t layer_index, fw64Texture* sprites, size_t sprite_count);

/**
 * Draws all frames of a sprite.
 * Useful for rendering a texture that does not fit into a single frame.
 * The x, y position specifies the top left position of the sprite.
 */
void fw64_sprite_batch_draw_sprite(fw64SpriteBatch* sprite_batch, fw64Texture* texture, int x, int y);

/**
 * Draws a single sprite frame.
 * The x, y position specifies the top left position of the sprite.
 */
void fw64_sprite_batch_draw_sprite_slice(fw64SpriteBatch* sprite_batch, fw64Texture* texture, int frame, int x, int y);

/**
 * Draws a sprite frame with the supplied transform values.
 * The x, y position specifies the top left position of the sprite.
 * Note: the rotation value is currently ignored.
 */
void fw64_sprite_batch_draw_sprite_slice_transform(fw64SpriteBatch* sprite_batch, fw64Texture* texture, int frame, int x, int y, float scale_x , float scale_y, float rotation);

// Rectangle Rendering
int fw64_sprite_batch_init_rect_layer(fw64SpriteBatch* sprite_batch, size_t layer_index, fw64Texture* sprites, int texture_count);

void fw64_sprite_batch_draw_filled_rect(fw64SpriteBatch* sprite_batch, int x, int y, int width, int height);

#ifdef __cplusplus
}
#endif
