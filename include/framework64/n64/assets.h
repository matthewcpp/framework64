#ifndef FW64_N64_ASSETS_H
#define FW64_N64_ASSETS_H

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

struct fw64Assets{
    fw64Asset* _assets;
    uint32_t _asset_count;
    uint32_t _asset_capacity;
};

#endif