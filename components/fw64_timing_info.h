#pragma once

#include <framework64/render_pass.h>
#include <framework64/sprite_batch.h>
#include <framework64/time.h>

typedef struct {
    fw64Font* font;
    IVec2 position;
    const fw64Time* time;
    int display_total_time;
} fw64TimingInfo;

#ifdef __cplusplus
extern "C" {
#endif

void fw64_timing_info_init(fw64TimingInfo* info, fw64Font* font, fw64Time* time);
void fw64_timing_info_to_spritebatch(fw64TimingInfo* info, fw64SpriteBatch* spritebatch);

#ifdef __cplusplus
}
#endif
