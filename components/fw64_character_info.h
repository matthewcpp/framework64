#pragma once

#include "fw64_character.h"

#include <framework64/font.h>

typedef struct {
    fw64Font* font;
    IVec2 position;
    fw64Character* character;
} fw64CharacterInfo;

#ifdef __cplusplus
extern "C" {
#endif

void fw64_character_info_init(fw64CharacterInfo* info, fw64Font* font, fw64Character* character);
void fw64_character_info_to_spritebatch(fw64CharacterInfo* info, fw64SpriteBatch* spritebatch);

#ifdef __cplusplus
}
#endif
