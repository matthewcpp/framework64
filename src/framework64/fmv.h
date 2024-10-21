#pragma once

#include "framework64/asset_database.h"

typedef struct fw64Fmv fw64Fmv;

typedef enum {
    /** No fmv file has been loaded */
    FW64_FMV_CLOSED,

    /** File has been loaded but is not playing */
    FW64_FMV_STOPPED,

    /** File has been loaded and is currently playing */
    FW64_FMV_PLAYING,

    /** File loaded, playback halted */
    FW64_FMV_PAUSED,

    /** Playback has completed */
    FW64_FMV_PLAYBACK_COMPLETE
} fw64FmvStatus;

#ifdef __cplusplus
extern "C" {
#endif

fw64FmvStatus fw64_fmv_get_status(fw64Fmv* fmv);
/**
 * Opens a video asset. The initial state of the video player will be stopped.
 * Retuns nonzeo on success or 0 on failure.
 */
int fw64_fmv_open(fw64Fmv* fmv, fw64AssetId asset_id);
void fw64_fmv_close(fw64Fmv* fmv);
void fw64_fmv_play(fw64Fmv* fmv);
void fw64_fmv_pause(fw64Fmv* fmv);

#ifdef __cplusplus
}
#endif
