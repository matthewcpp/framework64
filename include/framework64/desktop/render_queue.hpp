#pragma once

#include "framework64/animation_controller.h"
#include "framework64/mesh_instance.h"
#include "framework64/desktop/render_queue.hpp"
#include "framework64/skinned_mesh_instance.h"
#include "framework64/desktop/sprite_batch.hpp"

#include <vector>

namespace framework64 {

struct RenderQueue {

    std::vector<fw64SpriteBatch*> sprite_batches;
    std::vector<fw64MeshInstance*> mesh_instances;
    std::vector<fw64SkinnedMeshInstance*> skinned_mesh_instances;

    void clear() {
        sprite_batches.clear();
        mesh_instances.clear();
        skinned_mesh_instances.clear();
    }
};

}