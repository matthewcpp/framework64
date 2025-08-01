#include "fw64_skybox.h"

void fw64_skybox_init(fw64Skybox* skybox, fw64Engine* engine, fw64Scene* scene, fw64AssetId skybox_id, fw64Transform* target, fw64Allocator* allocator) {
    fw64Display* display = fw64_displays_get_primary(engine->displays);
    skybox->target = target;

    // load the skybox mesh
    skybox->mesh_node = fw64_scene_create_node(scene);
    fw64Mesh* mesh = fw64_scene_load_mesh_asset(scene, skybox_id);
    fw64MeshInstance* mesh_instance = fw64_scene_create_mesh_instance(scene, skybox->mesh_node, mesh);

    // temp tweak materials
    fw64MaterialCollection* materials = fw64_mesh_get_material_collection(mesh);

    for (uint32_t i = 0; i < materials->size; i++) {
        fw64Material* material = materials->materials[i];
        fw64_texture_set_wrap_mode(fw64_material_get_texture(material), FW64_TEXTURE_WRAP_CLAMP, FW64_TEXTURE_WRAP_CLAMP);
    }

    // setup the skybox camera
    skybox->camera_node = fw64_scene_create_node(scene);
    fw64_camera_init(&skybox->camera, skybox->camera_node, display);
    skybox->camera.near = 0.1f;
    skybox->camera.far = 10.0f;
    fw64_camera_update_projection_matrix(&skybox->camera);

    // configure the renderpass
    skybox->renderpass = fw64_renderpass_create(display, allocator);
    fw64_renderpass_set_depth_writing_enabled(skybox->renderpass, 0);
    fw64_renderpass_set_depth_testing_enabled(skybox->renderpass, 0);

    fw64_renderpass_begin(skybox->renderpass);
    fw64_renderpass_draw_static_mesh(skybox->renderpass, mesh_instance);
    fw64_renderpass_end(skybox->renderpass);
}

void fw64_skybox_uninit(fw64Skybox* skybox) {
    fw64_renderpass_delete(skybox->renderpass);
}

void fw64_skybox_update(fw64Skybox* skybox) {
    skybox->camera_node->transform.rotation = skybox->target->rotation;
    fw64_camera_update_view_matrix(&skybox->camera);
    fw64_renderpass_set_camera(skybox->renderpass, &skybox->camera);
}
