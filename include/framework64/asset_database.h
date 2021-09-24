#ifndef FW64_ASSETS_H
#define FW64_ASSETS_H

#include "framework64/audio_bank.h"
#include "framework64/font.h"
#include "framework64/image.h"
#include "framework64/mesh.h"

typedef struct fw64AssetDatabase fw64AssetDatabase;

#ifdef __cplusplus
extern "C" {
#endif

fw64Mesh* fw64_assets_get_mesh(fw64AssetDatabase* assets, uint32_t index);
fw64Font* fw64_assets_get_font(fw64AssetDatabase* assets, uint32_t index);
fw64Image* fw64_assets_get_image(fw64AssetDatabase* assets, uint32_t index);
fw64SoundBank* fw64_assets_get_sound_bank(fw64AssetDatabase* assets, uint32_t index);
fw64MusicBank* fw64_assets_get_music_bank(fw64AssetDatabase* assets, uint32_t index);

#ifdef __cplusplus
}
#endif

#endif