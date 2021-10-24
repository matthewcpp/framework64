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
    std::vector<std::unique_ptr<fw64Collider>> colliders;
    std::unordered_map<std::string, std::unique_ptr<framework64::CollisionMesh>> mesh_colliders;

    fw64Collider* createCollider() {
        colliders.emplace_back(new fw64Collider());
        return colliders.back().get();
    }

    fw64Node* createNode() {
        nodes.emplace_back(new fw64Node());
        return nodes.back().get();
    }

};

