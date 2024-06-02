#pragma once

#include "framework64/animation_controller.h"
#include "framework64/desktop/mesh.hpp"
#include "framework64/desktop/render_queue.hpp"
#include "framework64/desktop/sprite_batch.hpp"

#include <vector>

namespace framework64 {

struct fw64MeshInstance {
    fw64Mesh* mesh;
    fw64Transform* transform;

    fw64MeshInstance( fw64Mesh* mesh, fw64Transform* transform)
        : mesh(mesh), transform(transform) {}
};


struct fw64AnimatedMeshInstance {
    fw64Mesh* mesh;
    fw64AnimationController* controller;
    fw64Transform* transform;

    fw64AnimatedMeshInstance(fw64Mesh* mesh, fw64AnimationController* controller, fw64Transform* transform)
        : mesh(mesh), controller(controller), transform(transform)  {}
};


struct RenderQueue {

    std::vector<fw64SpriteBatch*> sprite_batches;
    std::vector<fw64MeshInstance> mesh_instances;
    std::vector<fw64AnimatedMeshInstance> animated_mesh_instances;

    void clear() {
        sprite_batches.clear();
        mesh_instances.clear();
        animated_mesh_instances.clear();
    }
};

}