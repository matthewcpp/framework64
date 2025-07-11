#pragma once

#include <fw64_timing_info.h>

#include <framework64/engine.h>

typedef struct {
    fw64Engine* engine;
    fw64RenderPass* renderpass;
    fw64SpriteBatch* spritebatch;
    fw64TimingInfo timing_info;
} UI;

void ui_init(UI* ui, fw64Engine* engine, fw64Font* font, fw64Allocator* allocator);
void ui_update(UI* ui);
void ui_draw(UI* ui);
void ui_uninit(UI* ui);
