#pragma once

#include "framework64/audio_bank.h"

#include <stdint.h>


typedef struct fw64AssetDatabase fw64AssetDatabase;

#ifdef __cplusplus
extern "C" {
#endif


fw64SoundBank* fw64_assets_get_sound_bank(fw64AssetDatabase* assets, uint32_t index);
fw64MusicBank* fw64_assets_get_music_bank(fw64AssetDatabase* assets, uint32_t index);

#ifdef __cplusplus
}
#endif
