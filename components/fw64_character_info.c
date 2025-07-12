#include "fw64_character_info.h"

#include <stdio.h>

void fw64_character_info_init(fw64CharacterInfo* info, fw64Font* font, fw64Character* character){
    info->font = font;
    info->character = character;
    ivec2_set_zero(&info->position);
}

void fw64_character_info_to_spritebatch(fw64CharacterInfo* info, fw64SpriteBatch* spritebatch) {
    IVec2 draw_pos = info->position;
    char buffer[64];

    sprintf(buffer, "p: %.2f, %.2f, %2.f", info->character->position.x, info->character->position.y, info->character->position.z);
    fw64_spritebatch_draw_string(spritebatch, info->font, buffer, draw_pos.x, draw_pos.y);

    draw_pos.y += fw64_font_line_height(info->font);
    sprintf(buffer, "v: %.2f, %.2f, %.2f", info->character->velocity.x, info->character->velocity.y, info->character->velocity.z);
    fw64_spritebatch_draw_string(spritebatch, info->font, buffer, draw_pos.x, draw_pos.y);
}
