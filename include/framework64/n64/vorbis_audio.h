#pragma once

#define STB_VORBIS_NO_STDIO
#define STB_VORBIS_NO_PULLDATA_API
#define STB_VORBIS_HEADER_ONLY
#include "stb/stb_vorbis.c"

#include "framework64/data_io.h"

#define FW64_VORBIS_AUDIO_DATA_SIZE (16 * 1024)

typedef struct {
    stb_vorbis* music_file;
    fw64DataSource* datasource;
    unsigned char data_buffer[FW64_VORBIS_AUDIO_DATA_SIZE];
    unsigned char vorbis_alloc_buffer[FW64_VORBIS_AUDIO_DATA_SIZE];
} fw64N64VorbisAudio;

int fw64_audio_load_soundbank_asset(fw64N64VorbisAudio* vorbis_audio, fw64DataSource* datasource);