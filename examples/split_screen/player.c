#include "player.h"

#include "framework64/util/renderer_util.h"

void player_init(Player* player, fw64Engine* engine, fw64Scene* scene, int node_id, int index) {
    player->engine = engine;
    player->scene = scene;
    player->player_index = index;
    player->active = 0;

    player->current_weapon = NULL;
    fw64_transform_init(&player->weapon_transform);

    fw64_fps_camera_init(&player->fps_camera, engine->input, fw64_displays_get_primary(engine->displays));
    player->fps_camera.player_index = player->player_index;

    fw64_camera_init(&player->weapon_camera, fw64_displays_get_primary(engine->displays));
    vec3_zero(&player->weapon_camera.transform.position);
    fw64_camera_update_view_matrix(&player->weapon_camera);

    player->weapon_camera.near = 0.1f;
    player->weapon_camera.far = 125.0f;
    player->weapon_camera.fovy = 60.0f;
    fw64_camera_update_projection_matrix(&player->weapon_camera);

    player->node = fw64_scene_get_node(player->scene, node_id);

    Box* bounding = &player->node->collider->bounding;
    Vec3 position;
    box_center(bounding, &position);

    position.y += bounding->max.y - bounding->min.y;
    player->fps_camera.camera.transform.position = position;
}

void player_update(Player* player) {
    fw64_fps_camera_update(&player->fps_camera, player->engine->time->time_delta);
}

void player_set_weapon(Player* player, Weapon* weapon) {
    player->current_weapon = weapon;

    player->weapon_transform.position = weapon->position;
    player->weapon_transform.rotation = weapon->rotation;
    player->weapon_transform.scale = weapon->scale;

    fw64_transform_update_matrix(&player->weapon_transform);
}

void player_draw_view(Player* player) {
    fw64_renderer_set_camera(player->engine->renderer, &player->fps_camera.camera);
    fw64_scene_draw_all(player->scene, player->engine->renderer);

    if (!player->current_weapon)
        return;

    fw64_renderer_set_camera(player->engine->renderer, &player->weapon_camera);
    fw64_renderer_util_clear_viewport(player->engine->renderer, &player->weapon_camera, FW64_RENDERER_FLAG_CLEAR_DEPTH);
    fw64_renderer_draw_static_mesh(player->engine->renderer, &player->weapon_transform, player->current_weapon->mesh);
}

void player_set_viewport_rect(Player* player, float x, float y, float w, float h) {
    Vec2 relative_pos = {x, y};
    Vec2 relative_size = {w, h};
    fw64_camera_set_viewport_relative(&player->fps_camera.camera, fw64_displays_get_primary(player->engine->displays), &relative_pos, &relative_size);
    fw64_camera_set_viewport_relative(&player->weapon_camera, fw64_displays_get_primary(player->engine->displays), &relative_pos, &relative_size);
}