#pragma once

#include "framework64/font.h"

#include "player.h"

typedef struct {
    fw64Engine* engine;
    fw64Font* font;
    Player* player;
    char status_text[64];
} UI;

#ifdef __cplusplus
extern "C" {
#endif

void ui_init(UI* ui, fw64Engine* engine, Player* player);
void ui_update(UI* ui);
void ui_draw(UI* ui);

#ifdef __cplusplus
}
#endif
