#pragma once

#include <framework64/sprite_batch.h>
#include <framework64/util/bump_allocator.h>

typedef struct {
    const fw64BumpAllocator* bump_allocator;
    fw64Font* font;
    IVec2 position;
} fw64BumpAllocatorInfo;

#ifdef __cplusplus
extern "C" {
#endif

void fw64_bump_allocator_info_init(fw64BumpAllocatorInfo* info, const fw64BumpAllocator* bump_allocator, fw64Font* font);
void fw64_bump_allocator_info_to_spritebatch(fw64BumpAllocatorInfo* info, fw64SpriteBatch* spritebatch);

#ifdef __cplusplus
}
#endif
