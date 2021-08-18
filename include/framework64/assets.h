#ifndef FW64_ASSETS_H
#define FW64_ASSETS_H

#include "framework64/font.h"
#include "framework64/mesh.h"
#include "framework64/texture.h"

typedef struct fw64Assets fw64Assets;

void fw64_assets_init(fw64Assets* assets);

int fw64_assets_is_loaded(fw64Assets* assets, uint32_t index);
fw64Mesh* fw64_assets_get_mesh(fw64Assets* assets, uint32_t index);
fw64Font* fw64_assets_get_font(fw64Assets* assets, uint32_t index);
fw64Texture* fw64_assets_get_image(fw64Assets* assets, uint32_t index);

#endif