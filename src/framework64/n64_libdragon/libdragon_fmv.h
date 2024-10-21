#pragma once

#include "framework64/fmv.h"

#include "libdragon_display.h"

#include <libdragon.h>

typedef enum {
    VIDEO_PLAYER_FLAG_NONE = 0,
    VIDEO_PLAYER_FLAG_LOOP =  1 << 0,
    VIDEO_PLAYER_NEEDS_NEXT_FRAME = 1 << 1
} VideoPlayerFlags;

struct fw64Fmv {
    fw64Displays* displays;
    mpeg2_t* video_track;
    yuv_blitter_t yuv;
    uint32_t flags;
    fw64FmvStatus status;
};

void fw64_libdragon_fmv_init(fw64Fmv* fmv, fw64Displays* displays);
void fw64_libdragon_fmv_update(fw64Fmv* fmv);
void fw64_libdragon_fmv_draw(fw64Fmv* fmv);