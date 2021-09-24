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
