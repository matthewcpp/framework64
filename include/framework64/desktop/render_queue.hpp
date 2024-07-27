#pragma once

#include "framework64/animation_controller.h"
#include "framework64/mesh_instance.h"
#include "framework64/desktop/render_queue.hpp"
#include "framework64/skinned_mesh_instance.h"
#include "framework64/desktop/sprite_batch.hpp"

#include <vector>

namespace framework64 {

struct RenderQueue {
    RenderQueue();
    
    void clear();
    void enqueueSpriteBatch(fw64SpriteBatch* sprite_batch);
    void enqueueMeshInstance(fw64MeshInstance* mesh_instance);
    void enqueueSkinnedMeshInstance(fw64SkinnedMeshInstance* skinned_mesh_instance);

    inline bool hasActiveQueueIndex(fw64MeshRenderQueueIndex index) const {
        return active_queue_indices[index];
    }

    std::vector<fw64SpriteBatch*> sprite_batches;
    std::vector<fw64MeshInstance*> mesh_instances;
    std::vector<fw64SkinnedMeshInstance*> skinned_mesh_instances;

    std::array<bool, FW64_RENDER_QUEUE_INDEX_COUNT> active_queue_indices;

    
};

}