#pragma once

#include "framework64/sprite_batch.h"
#include "framework64/vec2.h"

#include "framework64/util/dynamic_vector.h"

#include <libdragon.h>

typedef struct {
    Vec2 pos;
    fw64Texture* texture;
} SpriteBlitInfo;

struct fw64SpriteBatch {
    fw64Allocator* allocator;
    fw64DynamicVector blit_vec;
};