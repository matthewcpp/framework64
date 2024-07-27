#include "framework64/desktop/render_queue.hpp"
#include "framework64/desktop/mesh.hpp"

#include <algorithm>

namespace framework64 {

RenderQueue::RenderQueue() {
    clear();
}

void RenderQueue::clear() {
    sprite_batches.clear();
    mesh_instances.clear();
    skinned_mesh_instances.clear();

    std::fill(active_queue_indices.begin(), active_queue_indices.end(), false);
}

void RenderQueue::enqueueSpriteBatch(fw64SpriteBatch* sprite_batch) {
        sprite_batches.push_back(sprite_batch);
}

void RenderQueue::enqueueMeshInstance(fw64MeshInstance* mesh_instance) {
    mesh_instances.push_back(mesh_instance);
    active_queue_indices[mesh_instance->mesh->render_queue_index] = true;
}

void RenderQueue::enqueueSkinnedMeshInstance(fw64SkinnedMeshInstance* skinned_mesh_instance) {
    skinned_mesh_instances.push_back(skinned_mesh_instance);
    active_queue_indices[skinned_mesh_instance->mesh_instance.mesh->render_queue_index] = true;
}

}