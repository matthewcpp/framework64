#pragma once

#include <framework64/engine.h>
#include <framework64/scene.h>

/**
 * Texture 6x1 Frames: Front, Left, Back, Right, Top, Bottom
 */
typedef struct {
    fw64Mesh* mesh;
    fw64Node* mesh_node;
    fw64Node* camera_node;
    fw64RenderPass* renderpass;
    fw64Transform* target;

    fw64Camera camera;
} fw64Skybox;

#ifdef __cplusplus
extern "C" {
#endif

void fw64_skybox_init(fw64Skybox* skybox, fw64Engine* engine, fw64Scene* scene, fw64AssetId skybox_id, fw64Transform* target, fw64Allocator* allocator);
void fw64_skybox_uninit(fw64Skybox* skybox);
void fw64_skybox_update(fw64Skybox* skybox);

#ifdef __cplusplus
}
#endif
