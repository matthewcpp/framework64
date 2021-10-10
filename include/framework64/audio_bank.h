#pragma once

/** \file audio_bank.h */

#include "framework64/asset_database.h"

#include <stdint.h>

typedef struct fw64Audio fw64Audio;
typedef struct fw64SoundBank fw64SoundBank;
typedef struct fw64MusicBank fw64MusicBank;

#ifdef __cplusplus
extern "C" {
#endif

fw64SoundBank* fw64_sound_bank_load(fw64AssetDatabase* assets, uint32_t handle);
fw64MusicBank* fw64_music_bank_load(fw64AssetDatabase* assets, uint32_t handle);

#ifdef __cplusplus
}
#endif
