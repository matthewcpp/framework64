#pragma once

#include "framework64/font.h"
#include "framework64/render_pass.h"
#include "framework64/sprite_batch.h"

#include "player.h"

typedef struct {
    fw64Engine* engine;
    fw64Font* font;
    Player* player;
    
    fw64SpriteBatch* spritebatch;
} UI;

#ifdef __cplusplus
extern "C" {
#endif

void ui_init(UI* ui, fw64Engine* engine, Player* player);
void ui_update(UI* ui);

#ifdef __cplusplus
}
#endif
