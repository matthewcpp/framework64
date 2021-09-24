#include "framework64/asset_database.h"
#include "framework64/n64/asset_database.h"

#include "framework64/n64/audio_bank.h"
#include "framework64/n64/font.h"
#include "framework64/n64/image.h"
#include "framework64/n64/mesh.h"
#include "framework64/n64/texture.h"


#include <malloc.h>
#include <stddef.h>

#define FW64_ASSET_INITIAL_CAPACITY 8
#define FW64_ASSET_GROW_AMOUNT 8

void fw64_n64_assets_init(fw64AssetDatabase* assets) {
    assets->_asset_capacity = FW64_ASSET_INITIAL_CAPACITY;
    assets->_assets = calloc(FW64_ASSET_INITIAL_CAPACITY, sizeof(fw64Asset));
    assets->_asset_count = 0;
}

static void fw64_insert_asset(fw64AssetDatabase* assets, void* ptr, uint32_t index) {
    if (assets->_asset_count == assets->_asset_capacity) {
        assets->_asset_capacity += FW64_ASSET_GROW_AMOUNT;
        assets->_assets = realloc(assets->_assets, assets->_asset_capacity * sizeof(fw64Asset));
    }

    fw64Asset* asset = assets->_assets + assets->_asset_count;
    asset->ptr.any = ptr;
    asset->index = index;

    assets->_asset_count += 1;
}

static void* fw64_find_asset(fw64AssetDatabase* assets, uint32_t index) {
    for (size_t i = 0; i < assets->_asset_count; i++) {
        if (assets->_assets[i].index == index) {
            return assets->_assets[i].ptr.any;
        }
    }

    return NULL;
}

fw64SoundBank* fw64_assets_get_sound_bank(fw64AssetDatabase* assets, uint32_t index) {
    fw64SoundBank* sound_bank = fw64_find_asset(assets, index);

    if (sound_bank) 
        return sound_bank;

    sound_bank = fw64_n64_sound_bank_load(index);

    if (sound_bank)
        fw64_insert_asset(assets, sound_bank, index);

    return sound_bank;
}

fw64MusicBank* fw64_assets_get_music_bank(fw64AssetDatabase* assets, uint32_t index) {
    fw64MusicBank* music_bank = fw64_find_asset(assets, index);

    if (music_bank)
        return music_bank;

    music_bank = fw64_n64_music_bank_load(index);

    if (music_bank)
        fw64_insert_asset(assets, music_bank, index);

    return music_bank;
}
