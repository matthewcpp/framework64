#pragma once

#include "framework64/scene.h"

#include "framework64/n64/loader.h"

typedef struct {
    uint32_t mesh_count;
    uint32_t extras_count;
} fw64SceneInfo;

struct fw64Scene {
    fw64SceneInfo info;
    fw64Transform transform;
    fw64Mesh* meshes;
    fw64SceneExtra* extras;
    fw64MeshResources* mesh_resources;
    int debug;
};
