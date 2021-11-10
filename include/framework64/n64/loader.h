#pragma once

#include "framework64/n64/mesh.h"

typedef struct {
    fw64MeshResources* resources;
} fw64N64Loader;

void fw64_n64_loader_init(fw64N64Loader* loader);
void fw64_n64_loader_uninit(fw64N64Loader* loader);

void fw64_n64_loader_get_texture(fw64N64Loader* loader, fw64Texture* texture, uint32_t index);

/** Precondition: Allocator should not be NULL */
void fw64_n64_loader_load_mesh_resources(fw64N64Loader* loader, int handle, fw64Allocator* allocator);

/** Precondition: Allocator should not be NULL */
void fw64_n64_loader_load_mesh(fw64N64Loader* loader, fw64Mesh* mesh, int handle, fw64Allocator* allocator);
