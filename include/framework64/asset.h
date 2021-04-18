#ifndef FW64_ASSET_CACHE_H
#define FW64_ASSET_CACHE_H

#include "framework64/font.h"
#include "framework64/mesh.h"
#include "framework64/texture.h"

typedef union {
    void* any;
    fw64Font* font;
    fw64Texture* image;
    fw64Mesh* mesh;
} fw64AssetPtr;

typedef struct {
    fw64AssetPtr ptr;
    uint32_t index;
} fw64Asset;

typedef struct {
    fw64Asset* _assets;
    uint32_t _asset_count;
    uint32_t _asset_capacity;
} fw64Assets;

void fw64_assets_init(fw64Assets* assets);

int fw64_assets_is_loaded(fw64Assets* assets, uint32_t index);
fw64Mesh* fw64_assets_get_mesh(fw64Assets* assets, uint32_t index);
fw64Font* fw64_assets_get_font(fw64Assets* assets, uint32_t index);
fw64Texture* fw64_assets_get_image(fw64Assets* assets, uint32_t index);

#endif