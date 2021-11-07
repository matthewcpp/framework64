#pragma once

#include "framework64/desktop/collider.h"
#include "framework64/desktop/mesh.h"
#include "framework64/scene.h"

#include <memory>
#include <unordered_map>

struct fw64Scene {
    std::vector<std::unique_ptr<fw64Mesh>> meshes;
    std::vector<std::unique_ptr<fw64Node>> nodes;
    std::vector<std::unique_ptr<fw64Collider>> colliders;
    std::vector<std::unique_ptr<framework64::CollisionMesh>> collision_meshes;
    framework64::SharedResources shared_resources;
    Box bounding_box;

    fw64Collider* createCollider() {
        colliders.emplace_back(new fw64Collider());
        return colliders.back().get();
    }

    fw64Node* createNode() {
        nodes.emplace_back(new fw64Node());
        return nodes.back().get();
    }

    void calculateBounding();

};

