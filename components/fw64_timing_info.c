#include "fw64_timing_info.h"

#include <stdio.h>

void fw64_timing_info_init(fw64TimingInfo* info, fw64Font* font, fw64Time* time) {
    info->font = font;
    info->time = time;
    info->display_total_time = 0;
    ivec2_set_zero(&info->position);
}


void fw64_timing_info_to_spritebatch(fw64TimingInfo* info, fw64SpriteBatch* spritebatch) {
    char buffer[64];

    IVec2 draw_pos = info->position;

    sprintf(buffer, "td: %.3f", info->time->time_delta);
    fw64_spritebatch_draw_string(spritebatch, info->font, buffer, draw_pos.x, draw_pos.y);
}
