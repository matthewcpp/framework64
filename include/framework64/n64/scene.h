#pragma once

#include "framework64/scene.h"

#include "framework64/n64/loader.h"

typedef struct {
    uint32_t node_count;
    uint32_t mesh_count;
    uint32_t collider_count;
    uint32_t collision_mesh_count;
} fw64SceneInfo;

struct fw64Scene {
    fw64SceneInfo info;
    fw64Node* nodes;
    fw64Collider* colliders;
    fw64Mesh* meshes;
    fw64CollisionMesh* collision_meshes;
    fw64MeshResources* mesh_resources;
    fw64Allocator* allocator;
    Box bounding_box;
};
