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

/**
 * Loads sound bank data from the asset database.
 * Before you can play sounds contained in this bank you neeed to call \ref fw64_audio_set_sound_bank
 */
fw64SoundBank* fw64_sound_bank_load(fw64AssetDatabase* assets, uint32_t handle);

/**
 * Frees all resources used by and deletes a sound bank loaded with \ref fw64_sound_bank_load.
 * You should ensure the sound bank is not active before calling this function.
 */
void fw64_sound_bank_delete(fw64AssetDatabase* assets, fw64SoundBank* sound_bank);

/**
 * Loads a music bank from the asset database.
 * Before you can play music contained in this bank you need to call \ref fw64_audio_set_music_bank.
 */
fw64MusicBank* fw64_music_bank_load(fw64AssetDatabase* assets, uint32_t handle);

/**
 * Frees all resources used by and deletes a music bank loaded with \ref fw64_music_bank_load.
 * You should ensure the sound bank is not active before calling this function.
 */
void fw64_music_bank_delete(fw64AssetDatabase* assets, fw64MusicBank* music_bank);

#ifdef __cplusplus
}
#endif
