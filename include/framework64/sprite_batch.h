#pragma once

#include <framework64/font.h>

typedef struct fw64SpriteBatch fw64SpriteBatch;
typedef struct fw64SpriteBatchTextLayer fw64SpriteBatchTextLayer;

typedef enum {
    FW64_SPRITE_BATCH_LAYER_TYPE_UNKNOWN,
    FW64_SPRITE_BATCH_LAYER_TYPE_TEXT
} fw64SpriteBatchLayerType;

#ifdef __cplusplus
extern "C" {
#endif

void fw64_sprite_batch_init(fw64SpriteBatch* sprite_batch, int layer_count);
fw64SpriteBatchLayerType fw64_sprite_batch_get_layer_type(fw64SpriteBatch* sprite_batch);
int fw64_sprite_batch_get_layer_count(fw64SpriteBatch* sprite_batch);

fw64SpriteBatchTextLayer* fw64_sprite_batch_init_text_layer(fw64SpriteBatch* sprite_batch, fw64Font* font);

void fw64_sprite_batch_draw_string(fw64SpriteBatchTextLayer* layer, int x, int y, const char* text);
void fw64_sprite_batch_draw_string_count(fw64SpriteBatchTextLayer* layer, int x, int y, const char* text, uint32_t count);

#ifdef __cplusplus
}
#endif
