#include "player.h"

#include "framework64/util/renderer_util.h"

#include "assets/layers.h"

void player_init(Player* player, fw64Engine* engine, fw64Scene* scene, int node_id, int index) {
    fw64Display* display = fw64_displays_get_primary(engine->displays);

    player->engine = engine;
    player->scene = scene;
    player->player_index = index;

    player->weapon = NULL;
    player->scene_renderpass = fw64_renderpass_create(display, fw64_default_allocator());

    player->weapon_renderpass = fw64_renderpass_create(display, fw64_default_allocator());
    fw64_renderpass_set_clear_flags(player->weapon_renderpass, FW64_CLEAR_FLAG_DEPTH);

    player->camera_node = fw64_scene_create_node(scene);
    fw64_camera_init(&player->camera, player->camera_node, display);

    fw64_fps_camera_init(&player->fps_camera, engine->input, &player->camera);
    player->fps_camera.player_index = player->player_index;

    player->node = fw64_scene_get_node(player->scene, node_id);
    player->weapon_node = fw64_scene_create_node(player->scene);
    player->weapon_node->layer_mask = FW64_layer_weapon;

    Box* bounding = &player->node->collider->bounding;
    Vec3 position = player->node->transform.position;
    position.y += bounding->max.y;
    player->camera_node->transform.position = position;
}

void player_update(Player* player) {
    fw64_fps_camera_update(&player->fps_camera, player->engine->time->time_delta);

    quat_from_euler(&player->node->transform.rotation, 0.0f, player->fps_camera.rotation.y, 0.0f);
    // TODO: This should go away when controller is implemented
    player->node->transform.position.x = player->camera_node->transform.position.x;
    player->node->transform.position.z = player->camera_node->transform.position.z;
    fw64_node_update(player->node);
}

void player_set_weapon(Player* player, Weapon* weapon) {
    player->weapon = weapon;

    fw64_renderpass_set_projection_matrix(player->weapon_renderpass, weapon->projection_matrix, NULL);
    fw64_renderpass_set_view_matrix(player->weapon_renderpass, weapon->view_matrix);

    if (player->weapon_node->mesh_instance) {
        fw64_mesh_instance_set_mesh(player->weapon_node->mesh_instance, weapon->mesh);
    } else {
        fw64_scene_create_mesh_instance(player->scene, player->weapon_node, weapon->mesh);
    }
}

void player_draw_view(Player* player) {
    fw64Frustum frustum;
    fw64_camera_extract_frustum_planes(&player->camera, &frustum);
    uint32_t layer_mask = player->node->layer_mask | FW64_layer_weapon;

    fw64_renderpass_begin(player->scene_renderpass);
    fw64_renderpass_set_camera(player->scene_renderpass, &player->camera);
    fw64_scene_draw_frustrum(player->scene, player->scene_renderpass, &frustum, ~layer_mask);
    fw64_renderpass_end(player->scene_renderpass);

    fw64_renderer_submit_renderpass(player->engine->renderer, player->scene_renderpass);
}

void player_draw_weapon(Player* player) {
    if (!player->weapon) {
        return;
    }

    fw64_renderpass_begin(player->weapon_renderpass);
    fw64_renderpass_draw_static_mesh(player->weapon_renderpass, player->weapon_node->mesh_instance);
    fw64_renderpass_end(player->weapon_renderpass);

    fw64_renderer_submit_renderpass(player->engine->renderer, player->weapon_renderpass);
}

void player_set_viewport_rect(Player* player, float x, float y, float w, float h) {
    Vec2 relative_pos = {x, y};
    Vec2 relative_size = {w, h};
    fw64_camera_set_viewport_relative(&player->camera, &relative_pos, &relative_size);

    fw64_renderpass_set_viewport(player->scene_renderpass, &player->camera.viewport);
    fw64_renderpass_set_viewport(player->weapon_renderpass, &player->camera.viewport);
}
