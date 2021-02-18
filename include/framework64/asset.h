#ifndef FW64_ASSET_CACHE_H
#define FW64_ASSET_CACHE_H

#include "framework64/font.h"
#include "framework64/mesh.h"
#include "framework64/sprite.h"

typedef union {
    void* any;
    Font* font;
    ImageSprite* image;
    Mesh* mesh;
} AssetPtr;

typedef struct {
    AssetPtr ptr;
    uint32_t index;
} Asset;

typedef struct {
    Asset* _assets;
    uint32_t _asset_count;
    uint32_t _asset_capacity;
} Assets;

void assets_init(Assets* assets);

int assets_is_loaded(Assets* assets, uint32_t index);
Mesh* assets_get_mesh(Assets* assets, uint32_t index);
Font* assets_get_font(Assets* assets, uint32_t index);
ImageSprite* assets_get_image(Assets* assets, uint32_t index);

#endif