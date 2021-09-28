#pragma once

#include "framework64/n64/mesh.h"

typedef struct {
    fw64MeshResources* resources;
} fw64N64Loader;

void fw64_n64_loader_init(fw64N64Loader* loader);
void fw64_n64_loader_uninit(fw64N64Loader* loader);

fw64Mesh* fw64_n64_loader_parse_static_mesh(fw64N64Loader* loader, uint32_t index);
void fw64_n64_loader_get_texture(fw64N64Loader* loader, fw64Texture* texture, uint32_t index);
