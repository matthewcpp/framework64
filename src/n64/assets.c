#include "framework64/assets.h"
#include "framework64/n64/assets.h"

#include "framework64/n64/font.h"
#include "framework64/n64/mesh.h"
#include "framework64/n64/texture.h"


#include <malloc.h>
#include <stddef.h>

#define FW64_ASSET_INITIAL_CAPACITY 8
#define FW64_ASSET_GROW_AMOUNT 8

void fw64_n64_assets_init(fw64Assets* assets) {
    assets->_asset_capacity = FW64_ASSET_INITIAL_CAPACITY;
    assets->_assets = calloc(FW64_ASSET_INITIAL_CAPACITY, sizeof(fw64Asset));
    assets->_asset_count = 0;
}

static void fw64_insert_asset(fw64Assets* assets, void* ptr, uint32_t index) {
    if (assets->_asset_count == assets->_asset_capacity) {
        assets->_asset_capacity += FW64_ASSET_GROW_AMOUNT;
        assets->_assets = realloc(assets->_assets, assets->_asset_capacity * sizeof(fw64Asset));
    }

    fw64Asset* asset = assets->_assets + assets->_asset_count;
    asset->ptr.any = ptr;
    asset->index = index;

    assets->_asset_count += 1;
}

static void* fw64_find_asset(fw64Assets* assets, uint32_t index) {
    for (size_t i = 0; i < assets->_asset_count; i++) {
        if (assets->_assets[i].index == index) {
            return assets->_assets[i].ptr.any;
        }
    }

    return NULL;
}

fw64Mesh* fw64_assets_get_mesh(fw64Assets* assets, uint32_t index) {
    fw64Mesh* mesh = fw64_find_asset(assets, index);

    if (!mesh) {
        mesh = malloc(sizeof(fw64Mesh));

        if (fw64_n64_mesh_load(index, mesh)) {
            fw64_insert_asset(assets, mesh, index);
        }
        else {
            free(mesh);
            mesh = NULL;
        }
    }

    return mesh;
}

fw64Font* fw64_assets_get_font(fw64Assets* assets, uint32_t index) {
    fw64Font* font = fw64_find_asset(assets, index);

    if (!font) {
        font = malloc(sizeof(fw64Font));

        if (fw64_font_load(index, font)) {
            fw64_insert_asset(assets, font, index);
        }
        else{
            free(font);
            font = NULL;
        }
    }

    return font;
}

fw64Texture* fw64_assets_get_image(fw64Assets* assets, uint32_t index) {
    fw64Texture* image = fw64_find_asset(assets, index);

    if (!image) {
        image = malloc(sizeof(fw64Texture));

        if (fw64_n64_texture_load(index, image)) {
            fw64_insert_asset(assets, image, index);
        }
        else {
            free(image);
            image = NULL;
        }
    }

    return image;
}
