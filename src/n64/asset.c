#include "framework64/asset.h"

#include <malloc.h>
#include <stddef.h>

#define FW64_ASSET_INITIAL_CAPACITY 8
#define FW64_ASSET_GROW_AMOUNT 8

void assets_init(Assets* assets) {
    assets->_asset_capacity = FW64_ASSET_INITIAL_CAPACITY;
    assets->_assets = calloc(FW64_ASSET_INITIAL_CAPACITY, sizeof(Asset));
    assets->_asset_count = 0;
}

static void insert_asset(Assets* assets, void* ptr, uint32_t index) {
    if (assets->_asset_count == assets->_asset_capacity) {
        assets->_asset_capacity += FW64_ASSET_GROW_AMOUNT;
        assets->_assets = realloc(assets->_assets, assets->_asset_capacity * sizeof(Asset));
    }

    Asset* asset = assets->_assets + assets->_asset_count;
    asset->ptr.any = ptr;
    asset->index = index;

    assets->_asset_count += 1;
}

static void* find_asset(Assets* assets, uint32_t index) {
    for (size_t i = 0; i < assets->_asset_count; i++) {
        if (assets->_assets[i].index == index) {
            return assets->_assets[i].ptr.any;
        }
    }

    return NULL;
}

int assets_is_loaded(Assets* assets, uint32_t index) {
    return find_asset(assets, index) != NULL;
}

Mesh* assets_get_mesh(Assets* assets, uint32_t index) {
    Mesh* mesh = find_asset(assets, index);

    if (!mesh) {
        mesh = malloc(sizeof(Mesh));

        if (mesh_load(index, mesh)) {
            insert_asset(assets, mesh, index);
        }
        else {
            free(mesh);
            mesh = NULL;
        }
    }

    return mesh;
}

Font* assets_get_font(Assets* assets, uint32_t index) {
    Font* font = find_asset(assets, index);

    if (!font) {
        font = malloc(sizeof(Font));

        if (font_load(index, font)) {
            insert_asset(assets, font, index);
        }
        else{
            free(font);
            font = NULL;
        }
    }

    return font;
}

ImageSprite* assets_get_image(Assets* assets, uint32_t index) {
    ImageSprite* image = find_asset(assets, index);

    if (!image) {
        image = malloc(sizeof(ImageSprite));

        if (sprite_load(index, image)) {
            insert_asset(assets, image, index);
        }
        else {
            free(image);
            image = NULL;
        }
    }

    return image;
}
