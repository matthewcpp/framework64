#include "libdragon_sprite_batch.h"

#include <string.h>

fw64SpriteBatch* fw64_spritebatch_create(size_t layer_count, fw64Allocator* allocator){
    (void)layer_count;
    fw64SpriteBatch* spritebatch = fw64_allocator_malloc(allocator, sizeof(fw64SpriteBatch));
    spritebatch->allocator = allocator;
    fw64_dynamic_vector_init(&spritebatch->blit_vec, sizeof(SpriteBlitInfo), allocator);

    return spritebatch;
}

void fw64_spritebatch_delete(fw64SpriteBatch* spritebatch) {
    fw64_dynamic_vector_uninit(&spritebatch->blit_vec);
    fw64_allocator_free(spritebatch->allocator, spritebatch);
}

void fw64_spritebatch_begin(fw64SpriteBatch* spritebatch) {
    fw64_dynamic_vector_clear(&spritebatch->blit_vec);
}

void fw64_spritebatch_end(fw64SpriteBatch* spritebatch) {
    (void)spritebatch;
}

//Animation frame size defines
#define ANIM_FRAME_W 120
#define ANIM_FRAME_H 80

//Animation frame timing defines
#define ANIM_FRAME_DELAY 3
#define ANIM_FRAME_MAX 6

void fw64_spritebatch_draw_sprite_slice(fw64SpriteBatch* spritebatch, fw64Texture* texture, int frame, int x, int y) {
    SpriteBlitInfo* info = (SpriteBlitInfo*)fw64_dynamic_vector_emplace_back(&spritebatch->blit_vec);

    info->texture = texture;
    info->pos.x = (float)x;
    info->pos.y = (float)y;
}