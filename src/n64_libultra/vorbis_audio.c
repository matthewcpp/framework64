#include "framework64/n64/vorbis_audio.h"

#define STB_VORBIS_NO_STDIO
#define STB_VORBIS_NO_PULLDATA_API
#define STB_VORBIS_MAX_CHANNELS 2

#include "stb/stb_vorbis.c"

int fw64_audio_load_soundbank_asset(fw64N64VorbisAudio* vorbis_audio, fw64DataSource* datasource) {
    stb_vorbis_alloc alloc_buffer;
    alloc_buffer.alloc_buffer = NULL;
    alloc_buffer.alloc_buffer_length_in_bytes = FW64_VORBIS_AUDIO_DATA_SIZE;
    int num_bytes_consumed, error;

    vorbis_audio->datasource = datasource;
    vorbis_audio->music_file = stb_vorbis_open_pushdata(&vorbis_audio->data_buffer[0], FW64_VORBIS_AUDIO_DATA_SIZE, &num_bytes_consumed, &error, &alloc_buffer);

    return vorbis_audio->music_file != 0;
}