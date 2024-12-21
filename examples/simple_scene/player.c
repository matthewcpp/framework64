#include "player.h"

#include "framework64/controller_mapping/n64.h"

void player_init(Player* player, fw64Engine* engine, fw64Scene* scene, fw64Camera* camera, fw64AssetId mesh_index, Vec3* position) {
    player->engine = engine;
    player->scene = scene;

    player->node = fw64_scene_create_node(scene);
    fw64Mesh* mesh = fw64_scene_load_mesh_asset(scene, mesh_index);
    fw64_scene_create_mesh_instance(scene, player->node, mesh);
    Box bounding = fw64_mesh_get_bounding_box(mesh);
    fw64Collider* collider = fw64_scene_create_box_collider(scene, player->node, &bounding);

    fw64_third_person_controller_init(&player->character_controller, engine, scene, collider, camera);
    player_reset(player, position);
}

void player_reset(Player* player, Vec3* position) {
    fw64_character_controller_reset(&player->character_controller.base);

    quat_ident(&player->node->transform.rotation);

    if (position) {
        player->node->transform.position = *position;
        fw64_node_update(player->node);
    }
}

void player_fixed_update(Player* player) {
    fw64_third_person_controller_fixed_update(&player->character_controller);
}

void player_update(Player* player) {
    fw64_third_person_controller_update(&player->character_controller);
}

void player_calculate_size(Player* player) {
    Vec3 extents;
    box_extents(&player->node->collider->bounding, &extents);
    player->character_controller.base.height = extents.y * 2.0f;
    player->character_controller.base.radius = extents.x > extents.z ? extents.x : extents.z;
}
