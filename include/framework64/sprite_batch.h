#pragma once

/** \file sprite_batch.h */

#include "framework64/allocator.h"
#include "framework64/camera.h"
#include "framework64/font.h"
#include "framework64/texture.h"

#include <stdint.h>

typedef struct fw64SpriteBatch fw64SpriteBatch;

#ifdef __cplusplus
extern "C" {
#endif

fw64SpriteBatch* fw64_spritebatch_create(size_t layer_count, fw64Allocator* allocator);
void fw64_spritebatch_delete(fw64SpriteBatch* sprite_batch);

void fw64_spritebatch_set_color(fw64SpriteBatch* sprite_batch, uint8_t r, uint8_t g, uint8_t b, uint8_t a);

/** Call this to begin a new batch.  Any existing data will be cleared. */
void fw64_spritebatch_begin(fw64SpriteBatch* sprite_batch);

/** This method should be called after all sprites have been added but before the first time the batch is rendered. */
void fw64_spritebatch_end(fw64SpriteBatch* sprite_batch);

size_t fw64_spritebatch_get_layer_count(fw64SpriteBatch* sprite_batch);
int fw64_spritebatch_set_active_layer(fw64SpriteBatch* sprite_batch, size_t layer_index);

/**
 * Draws all frames of a sprite.
 * Useful for rendering a texture that does not fit into a single frame.
 * The x, y position specifies the top left position of the sprite.
 */
void fw64_spritebatch_draw_sprite(fw64SpriteBatch* sprite_batch, fw64Texture* texture, int x, int y);

/**
 * Draws a single sprite frame.
 * The x, y position specifies the top left position of the sprite.
 */
void fw64_spritebatch_draw_sprite_slice(fw64SpriteBatch* sprite_batch, fw64Texture* texture, int frame, int x, int y);

void fw64_spritebatch_draw_sprite_slice_rect(fw64SpriteBatch* sprite_batch, fw64Texture* texture, int frame, int x, int y, int width, int height);

void fw64_spritebatch_draw_string(fw64SpriteBatch* sprite_batch, fw64Font* font, const char* text, int x, int y);
void fw64_spritebatch_draw_string_count(fw64SpriteBatch* sprite_batch,fw64Font* font,  const char* text, uint32_t count, int x, int y);

#ifdef __cplusplus
}
#endif
