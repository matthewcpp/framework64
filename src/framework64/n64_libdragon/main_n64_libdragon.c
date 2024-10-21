#if 0
#include <libdragon.h>

// Number of frame back buffers we reserve.
// These buffers are used to render the video ahead of time.
// More buffers help ensure smooth video playback at the cost of more memory.
#define NUM_DISPLAY 8

// Maximum target audio frequency.
//
// Needs to be 48 kHz if Opus audio compression is used.
// In this example, we are using VADPCM audio compression
// which means we can use the real frequency of the audio track.
//#define AUDIO_HZ 32000.0f
#define AUDIO_HZ 48000.0f

// Target screen resolution that we render at.
// Choosing a resolution above 240p (interlaced) can't be recommended for video playback.
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

typedef enum {
    VIDEO_PLAYER_FLAG_NONE = 0,
    VIDEO_PLAYER_FLAG_LOOP =  1 << 0,
    VIDEO_PLAYER_NEEDS_NEXT_FRAME = 1 << 1
} VideoPlayerFlags;

typedef enum {
    VIDEO_PLAYER_STOPPED,
    VIDEO_PLAYER_PLAYING,
    VIDEO_PLAYER_COMPLETE
} VideoPlayerState;

typedef struct {
    mpeg2_t* video_track;
    yuv_blitter_t yuv;
    uint32_t flags;
    VideoPlayerState state;
} VideoPlayer;

void video_player_init(VideoPlayer* video_player, const char* video_file) {
    video_player->state = VIDEO_PLAYER_STOPPED;
    video_player->flags = VIDEO_PLAYER_NEEDS_NEXT_FRAME;

    // Open the movie using the mpeg2 module and create a YUV blitter to draw it.
	video_player->video_track = mpeg2_open(video_file);
	video_player->yuv = yuv_blitter_new_fmv(
		// Resolution of the video we expect to play.
		// Video needs to have a width divisible by 32 and a height divisible by 16.
		//
		// Here we have a video resolution of 288x160 which is a nice, valid resolution
		// that leaves a margin of 32 pixels on the side - great for making sure
		// CRT TVs with overscan still display the entire frame of your video.
		// The resolution is not an exact 16:9 ratio (16:8.88) but it's close enough that
		// most people won't notice. The lower resolution can also help with performance.
		mpeg2_get_width(video_player->video_track), mpeg2_get_height(video_player->video_track),
		// Set blitter's output area to our entire display
		display_get_width(), display_get_height(),
		// Override default FMV parms to not zoom the video.
		// This will leave our desired CRT TV-friendly margin around the video.
		&(yuv_fmv_parms_t) {.zoom = YUV_ZOOM_NONE}
	);
}

void video_player_uninit(VideoPlayer* video_player) {
    yuv_blitter_free(&video_player->yuv);
    mpeg2_close(video_player->video_track);
}

void video_player_play(VideoPlayer* video_player) {
    if (video_player->state == VIDEO_PLAYER_COMPLETE) {
        mpeg2_rewind(video_player->video_track);
        video_player->flags = VIDEO_PLAYER_NEEDS_NEXT_FRAME;
    }

    video_player->state = VIDEO_PLAYER_PLAYING;
}

int video_player_is_looping(VideoPlayer* video_player) {
    return video_player->flags & VIDEO_PLAYER_FLAG_LOOP;
}

void video_player_update(VideoPlayer* video_player) {
    if (video_player->state != VIDEO_PLAYER_PLAYING) {
        return;
    }

    int video_playing = mpeg2_next_frame(video_player->video_track);

    if (!video_playing) {
        if (video_player_is_looping(video_player)) {
            mpeg2_rewind(video_player->video_track);
            mpeg2_next_frame(video_player->video_track);
        } else {
            video_player->state = VIDEO_PLAYER_COMPLETE;
        }
    }

    video_player->flags &= ~VIDEO_PLAYER_NEEDS_NEXT_FRAME;
}

void video_player_draw(VideoPlayer* video_player) {
    if (video_player->state == VIDEO_PLAYER_COMPLETE) {
        return;
    }

    if (video_player->flags & VIDEO_PLAYER_NEEDS_NEXT_FRAME) {
        mpeg2_next_frame(video_player->video_track);
        video_player->flags &= ~VIDEO_PLAYER_NEEDS_NEXT_FRAME;
    }

    yuv_frame_t frame = mpeg2_get_frame(video_player->video_track);
    yuv_blitter_run(&video_player->yuv, &frame);
}

void video_player_set_loop(VideoPlayer* video_player, int loop) {
    if (loop) {
        video_player->flags |= VIDEO_PLAYER_FLAG_LOOP;
    } else {
        video_player->flags &= ~VIDEO_PLAYER_FLAG_LOOP;
    }
}

#define MOVIE_COUNT 5

const char* movie_files[MOVIE_COUNT] = {
    "rom:/robot10.m1v",
    "rom:/washable1.m1v",
    "rom:/ninja2.m1v",
    "rom:/water1.m1v",
    "rom:/wires1.m1v",
};

int main(void)
{
	joypad_init();
	debug_init_isviewer();
	debug_init_usblog();

	display_init((resolution_t)
	{
		.width = SCREEN_WIDTH,
		.height = SCREEN_HEIGHT,
		.interlaced = INTERLACE_OFF
	},
	// 32-bit display mode is mandatory for video playback.
	DEPTH_32_BPP,
	NUM_DISPLAY, GAMMA_NONE,
	// FILTERS_DISABLED disables all VI post-processing to achieve the sharpest
	// possible image. If you'd like to soften the image a little bit, switch to
	// FITLERS_RESAMPLE.
	FILTERS_DISABLED);

	dfs_init(DFS_DEFAULT_LOCATION);
	rdpq_init();
	//profile_init();
	yuv_init();

	audio_init(AUDIO_HZ, 4);
	mixer_init(8);

	// Check if the movie is present in the filesystem, so that we can provide
	// a specific error message.
	// FILE *f = fopen("rom:/movie.m1v", "rb");
	// assertf(f, "Movie not found!\nInstall wget and ffmpeg to download and encode the sample movie\n");
	// fclose(f);

    int movie_index = 0;

    VideoPlayer video_player;
    video_player_init(&video_player, movie_files[movie_index]);
    video_player_play(&video_player);

	// Engage the fps limiter to ensure proper video pacing.
	float fps = mpeg2_get_framerate(video_player.video_track);
	display_set_fps_limit(fps);

	// // Open the audio track and start playing it in channel 0.
	wav64_t audio_track;
	wav64_open(&audio_track, "rom:/moonlight_shadow1.wav64");
	mixer_ch_play(0, &audio_track.wave);

	int nframes = 0;

	while (1)
	{
		mixer_throttle(AUDIO_HZ / fps);

		// if (!mpeg2_next_frame(video_player.video_track))
		// {
		// 	mpeg2_rewind(video_player.video_track);
        //     if (!mpeg2_next_frame(video_player.video_track)) {
        //         break;
        //     }
		// }
        video_player_update(&video_player);
        if (video_player.state == VIDEO_PLAYER_COMPLETE) {
            video_player_uninit(&video_player);
            movie_index += 1;
            if (movie_index >= MOVIE_COUNT) {
                movie_index = 0;
            }
            video_player_init(&video_player, movie_files[movie_index]);
            video_player_play(&video_player);
        }

		// This polls the mixer to try and play the next chunk of audio, if available.
		// We call this function twice during the frame to make sure the audio never stalls.
		// mixer_try_play();

		rdpq_attach(display_get(), NULL);

		// PROFILE_START(PS_YUV, 0);
		// Get the next video frame and feed it into our previously set up blitter.
		// yuv_frame_t frame = mpeg2_get_frame(video_player.video_track);
		// yuv_blitter_run(&video_player.yuv, &frame);
		// PROFILE_STOP(PS_YUV, 0);
        video_player_draw(&video_player);

		rdpq_detach_show();

		nframes++;

		mixer_try_play();

		// PROFILE_START(PS_SYNC, 0);
		rspq_wait();
		// PROFILE_STOP(PS_SYNC, 0);

		// profile_next_frame();
		// if (nframes % 128 == 0)
		// {
		// 	profile_dump();
		// 	profile_init();
		// }
	}
}
#endif

#if 1
#include "framework64/n64_libdragon/libdragon_engine.h"

#include <libdragon.h>

#include "${game_include_path}"

fw64LibdragonEngine engine;
Game game;

int main()
{
    fw64_n64_libdragon_engine_init(&engine);
    game_init(&game, &engine.interface);
    
    while (1)
    {
        fw64_n64_libdragon_engine_update(&engine);
        game_update(&game);

        surface_t* disp = display_get();
        surface_t* zbuf = display_get_zbuf();
        rdpq_attach(disp, zbuf);

        fw64_n64_libdragon_engine_draw(&engine);
        game_draw(&game);
    
        rdpq_detach_show();
    }
}
#endif

#if 0
#include <libdragon.h>

extern FILE *must_fopen(const char *fn);
extern int must_open(const char *fn);

#include <malloc.h>
#include <unistd.h>
#include <sys/stat.h>

int main() 
{
    debug_init_isviewer();
    display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE);
    dfs_init(DFS_DEFAULT_LOCATION);

    int f = must_open("rom:/0");
    struct stat stat;
    fstat(f, &stat);
    int file_size = stat.st_size, buffer_size = 0;
    close(f);

    FILE* fd = must_fopen("rom:/0");
    asset_loadf_into(fd, &file_size, NULL, &buffer_size);
    void* asset_buffer = malloc((size_t)buffer_size);
    fseek(fd, 0, SEEK_SET);
    asset_loadf_into(fd, &file_size, asset_buffer, &buffer_size);
    sprite_t* my_sprite = (sprite_t*)asset_buffer;
    fclose(fd);

    (void)my_sprite;

    while (1) {
        surface_t* disp = display_get();
        surface_t* zbuf = display_get_zbuf();
        rdpq_attach(disp, zbuf);

        rdpq_detach_show();
    }
}

#endif