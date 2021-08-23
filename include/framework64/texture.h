#ifndef FW64_SPRITE_H
#define FW64_SPRITE_H

#include "framework64/vec2.h"

#include <stdint.h>

#define SPRITE_FLAG_DYNAMIC 1

typedef struct fw64Texture fw64Texture;

int fw64_texture_get_slice_width(fw64Texture* sprite);
int fw64_texture_get_slice_height(fw64Texture* sprite);

#endif
