#include "bomb_omb_battlefield.h"
#include "assets/assets.h"
#include "assets/layers.h"
#include "assets/scene_Bomb_Omb_Battlefield.h"

void bomb_omb_battlefield_init(BombOmbBattlefield* battlefield, fw64Engine* engine, fw64Allocator* allocator) {
    battlefield->engine = engine;

    //setup scene
    fw64_collision_scene_manager_init(&battlefield->collision_scene_manager, engine, fw64_displays_get_primary(engine->displays), allocator);
    fw64AssetId wireframe_scene_asset_id = 
    #ifdef FW64_PLATFORM_N64_LIBULTRA
        FW64_INVALID_ASSET_ID;
    #else
        FW64_ASSET_scene_Bomb_Omb_Battlefield_collision_wireframe;
    #endif
    fw64Scene* scene = fw64_collision_scene_manager_load_scene(&battlefield->collision_scene_manager, FW64_ASSET_scene_Bomb_Omb_Battlefield, wireframe_scene_asset_id , FW64_layer_world);

    // setup player
    fw64Node* player_node = fw64_scene_get_node(scene, FW64_scene_Bomb_Omb_Battlefield_node_player);
    fw64Node* camera_node = fw64_scene_get_node(scene, FW64_scene_Bomb_Omb_Battlefield_node_camera);
    fw64_character_envionment_init(&battlefield->character_environment);
    quat_from_euler(&player_node->transform.rotation, 0, 270.0f, 0.0f);
    player_init(&battlefield->player, engine, &battlefield->character_environment, scene, player_node, camera_node, allocator);

    battlefield->collision_scene_manager.target = &battlefield->player.node->transform;

    // setup skybox
    fw64_skybox_init(&battlefield->skybox, engine, scene, FW64_ASSET_mesh_skybox, &battlefield->player.third_person_cam.camera->node->transform, allocator);
    battlefield->skybox.mesh_node->layer_mask = FW64_layer_skybox;

    fw64_headlight_init(&battlefield->headlight, battlefield->collision_scene_manager.static_scene_renderpass, 0, &battlefield->player.camera.node->transform);

    // setup music
    fw64_audio_load_musicbank_asset(engine->audio, engine->assets, FW64_ASSET_musicbank_bomb_omb_battlefield_music);
    #ifndef COLLISION_TEST_NO_MUSIC
        fw64_audio_play_music(engine->audio, 0);
    #endif
}

void bomb_omb_battlefield_update(BombOmbBattlefield* battlefield) {
    player_update(&battlefield->player);
    fw64_headlight_update(&battlefield->headlight);
    fw64_skybox_update(&battlefield->skybox);
}

void bomb_omb_battlefield_fixed_update(BombOmbBattlefield* battlefield) {
    player_fixed_update(&battlefield->player);
}

void bomb_omb_battlefield_draw(BombOmbBattlefield* battlefield) {
    fw64_renderer_submit_renderpass(battlefield->engine->renderer, battlefield->skybox.renderpass);
    fw64_collision_scene_manager_set_camera(&battlefield->collision_scene_manager, &battlefield->player.camera);
    fw64_collision_scene_manager_draw_scene(&battlefield->collision_scene_manager, ~FW64_layer_skybox);
}
