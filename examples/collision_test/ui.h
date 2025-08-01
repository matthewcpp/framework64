#pragma once

#include "player.h"

#include <fw64_timing_info.h>
#include <fw64_character_info.h>
#include <fw64_bump_allocator_info.h>

#include <framework64/engine.h>
#include <framework64/util/bump_allocator.h>

typedef struct {
    fw64Engine* engine;
    fw64RenderPass* renderpass;
    fw64SpriteBatch* spritebatch;
    fw64BumpAllocator* bump_allocator;
    fw64TimingInfo timing_info;
    fw64CharacterInfo character_info;
    fw64BumpAllocatorInfo bump_info;
} UI;

void ui_init(UI* ui, fw64Engine* engine, fw64Font* font, Player* player, fw64BumpAllocator* bump_allocator);
void ui_update(UI* ui);
void ui_draw(UI* ui);
void ui_uninit(UI* ui);
