#include "framework64/asset.h"

#include <malloc.h>
#include <stddef.h>

#define FW64_ASSET_INITIAL_CAPACITY 8
#define FW64_ASSET_GROW_AMOUNT 8

void fw64_assets_init(fw64Assets* assets) {
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

int fw64_assets_is_loaded(fw64Assets* assets, uint32_t index) {
    return fw64_find_asset(assets, index) != NULL;
}

Mesh* fw64_assets_get_mesh(fw64Assets* assets, uint32_t index) {
    Mesh* mesh = fw64_find_asset(assets, index);

    if (!mesh) {
        mesh = malloc(sizeof(Mesh));

        if (mesh_load(index, mesh)) {
            fw64_insert_asset(assets, mesh, index);
        }
        else {
            free(mesh);
            mesh = NULL;
        }
    }

    return mesh;
}

Font* fw64_assets_get_font(fw64Assets* assets, uint32_t index) {
    Font* font = fw64_find_asset(assets, index);

    if (!font) {
        font = malloc(sizeof(Font));

        if (font_load(index, font)) {
            fw64_insert_asset(assets, font, index);
        }
        else{
            free(font);
            font = NULL;
        }
    }

    return font;
}

ImageSprite* fw64_assets_get_image(fw64Assets* assets, uint32_t index) {
    ImageSprite* image = fw64_find_asset(assets, index);

    if (!image) {
        image = malloc(sizeof(ImageSprite));

        if (sprite_load(index, image)) {
            fw64_insert_asset(assets, image, index);
        }
        else {
            free(image);
            image = NULL;
        }
    }

    return image;
}
