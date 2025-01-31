#include "libdragon_fmv.h"

#include "framework64/n64_libdragon/libdragon_asset_database.h"

void fw64_libdragon_fmv_init(fw64Fmv* fmv, fw64Displays* displays) {
    fmv->displays = displays;
    fmv->status = FW64_FMV_CLOSED;
    fmv->flags = VIDEO_PLAYER_FLAG_NONE;
}

fw64FmvStatus fw64_fmv_get_status(fw64Fmv* fmv) {
    return fmv->status;
}

int fw64_fmv_open(fw64Fmv* fmv, fw64AssetId asset_id) {
    if (fmv->status != FW64_FMV_CLOSED) {
        fw64_fmv_close(fmv);
    }

    // Open the movie using the mpeg2 module and create a YUV blitter to draw it.
    char fmv_path[16];
    fw64_libdragon_asset_database_filepath(asset_id, fmv_path);
    fmv->video_track = mpeg2_open(fmv_path);

    if (!fmv->video_track) {
        return 0;
    }

    // TODO: do we want to do this here?
    fmv->displays->fps = mpeg2_get_framerate(fmv->video_track);
    display_set_fps_limit(fmv->displays->fps);

    fmv->yuv = yuv_blitter_new_fmv(
        mpeg2_get_width(fmv->video_track), mpeg2_get_height(fmv->video_track),
        // Set blitter's output area to our entire display
        display_get_width(), display_get_height(),
        // Override default FMV parms to not zoom the video.
        // This will leave our desired CRT TV-friendly margin around the video.
        &(yuv_fmv_parms_t) {.zoom = YUV_ZOOM_NONE}
    );

    fmv->status = FW64_FMV_STOPPED;
    fmv->flags = VIDEO_PLAYER_NEEDS_NEXT_FRAME;

    return 1;
}

void fw64_fmv_close(fw64Fmv* fmv) {
    if (fmv->status == FW64_FMV_CLOSED) {
        return;
    }

    yuv_blitter_free(&fmv->yuv);
    mpeg2_close(fmv->video_track);
    fmv->status = FW64_FMV_CLOSED;
}

void fw64_libdragon_fmv_update(fw64Fmv* fmv) {
    if (fmv->status != FW64_FMV_PLAYING) {
        return;
    }

    int video_playing = mpeg2_next_frame(fmv->video_track);

    if (!video_playing) {
        fmv->status = FW64_FMV_PLAYBACK_COMPLETE;
    }

    fmv->flags &= ~VIDEO_PLAYER_NEEDS_NEXT_FRAME;
}

void fw64_libdragon_fmv_draw(fw64Fmv* fmv) {
    if (fmv->status == FW64_FMV_CLOSED || fmv->status == FW64_FMV_PLAYBACK_COMPLETE) {
        return;
    }

    if (fmv->flags & VIDEO_PLAYER_NEEDS_NEXT_FRAME) {
        mpeg2_next_frame(fmv->video_track);
        fmv->flags &= ~VIDEO_PLAYER_NEEDS_NEXT_FRAME;
    }

    yuv_frame_t frame = mpeg2_get_frame(fmv->video_track);
    yuv_blitter_run(&fmv->yuv, &frame);
}

void fw64_fmv_play(fw64Fmv* fmv) {
    switch (fmv->status) {
        case FW64_FMV_CLOSED:
        case FW64_FMV_PLAYING:
            return;

        case FW64_FMV_STOPPED:
        case FW64_FMV_PAUSED:
            break;

        case FW64_FMV_PLAYBACK_COMPLETE:
            mpeg2_rewind(fmv->video_track);
            fmv->flags = VIDEO_PLAYER_NEEDS_NEXT_FRAME;
            break;
    };

    fmv->status = FW64_FMV_PLAYING;
}

void fw64_fmv_pause(fw64Fmv* fmv) {
    if (fmv->status == FW64_FMV_PLAYING) {
        fmv->status = FW64_FMV_PAUSED;
    }
}