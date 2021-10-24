#pragma once

#include "framework64/desktop/collider.h"
#include "framework64/desktop/mesh.h"
#include "framework64/scene.h"

#include <memory>
#include <unordered_map>

struct fw64Scene{
    std::vector<std::unique_ptr<fw64Mesh>> meshes;
    std::vector<fw64Material*> materials;
    std::vector<fw64Texture*> textures;
    std::vector<fw64Image*> images;
    std::vector<std::unique_ptr<fw64Node>> nodes;
    std::unordered_map<std::string, std::unique_ptr<framework64::MeshCollider>> mesh_colliders;
};

