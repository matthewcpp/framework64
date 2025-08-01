#include "fw64_bump_allocator_info.h"

#include <stdio.h>

void fw64_bump_allocator_info_init(fw64BumpAllocatorInfo* info, const fw64BumpAllocator* bump_allocator, fw64Font* font) {
    info->bump_allocator = bump_allocator;
    info->font = font;
    ivec2_set_zero(&info->position);
}

void fw64_bump_allocator_info_to_spritebatch(fw64BumpAllocatorInfo* info, fw64SpriteBatch* spritebatch) {
    char buffer[32];
    const uint32_t committed = fw64_bump_allocator_committed(info->bump_allocator);
    sprintf(buffer, "mem: %u", committed);

    fw64_spritebatch_draw_string(spritebatch, info->font, buffer, info->position.x, info->position.y);
}
