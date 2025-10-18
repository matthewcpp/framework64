#pragma once

#include "player.h"

#include <fw64_character.h>
#include <fw64_collision_geometry_debug.h>

#include <framework64/engine.h>
#include <framework64/util/bump_allocator.h>

typedef struct {
    fw64Engine* engine;
    fw64Font* font;
    fw64RenderPass* renderpass;
    fw64SpriteBatch* spritebatch;
    fw64CollisionGeometryDebug* geometry_debug;
    fw64BumpAllocator* bump_allocator;
    Player* player;
    IVec2 position;
} UI;

void ui_init(UI* ui, fw64Engine* engine, fw64Font* font, Player* player, fw64CollisionGeometryDebug* geometry_debug, fw64BumpAllocator* bump_allocator);
void ui_update(UI* ui);
void ui_draw(UI* ui);
void ui_uninit(UI* ui);
