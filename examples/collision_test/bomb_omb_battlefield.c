#include "bomb_omb_battlefield.h"
#include "assets/assets.h"
#include "assets/layers.h"
#include "assets/scene_Bomb_Omb_Battlefield.h"
#include "assets/music_bank_bomb_omb_battlefield_music.h"

static void bomb_om_battlefield_on_star_triggered(void* arg);

void bomb_omb_battlefield_init(BombOmbBattlefield* battlefield, fw64Engine* engine, fw64Allocator* allocator) {
    battlefield->engine = engine;

    // setup scene
    battlefield->scene = fw64_assets_load_scene(engine->assets, FW64_ASSET_scene_Bomb_Omb_Battlefield, allocator);
    battlefield->scene_renderpass = fw64_renderpass_create(fw64_displays_get_primary(engine->displays), allocator);

    // setup player
    fw64Node* player_node = fw64_scene_get_node(battlefield->scene, FW64_scene_Bomb_Omb_Battlefield_node_player);
    fw64Node* camera_node = fw64_scene_get_node(battlefield->scene, FW64_scene_Bomb_Omb_Battlefield_node_camera);
    fw64_character_envionment_init(&battlefield->character_environment);
    quat_from_euler(&player_node->transform.rotation, 0, 270.0f, 0.0f);
    player_init(&battlefield->player, engine, &battlefield->character_environment, battlefield->scene, player_node, camera_node, allocator);

    // setup skybox
    fw64_skybox_init(&battlefield->skybox, engine, battlefield->scene, FW64_ASSET_mesh_skybox, &battlefield->player.third_person_cam.camera->node->transform, allocator);
    battlefield->skybox.mesh_node->layer_mask = FW64_layer_skybox;

    fw64_headlight_init(&battlefield->headlight, battlefield->scene_renderpass, 0, &battlefield->player.camera.node->transform);

    // setup music
    fw64_audio_load_musicbank_asset(engine->audio, engine->assets, FW64_ASSET_musicbank_bomb_omb_battlefield_music);
    #ifndef COLLISION_TEST_NO_MUSIC
        fw64_audio_play_music(engine->audio, music_bank_bomb_omb_battlefield_music_bomb_omb);
    #endif

    // setup star
    fw64Node* star_node = fw64_scene_get_node(battlefield->scene, FW64_scene_Bomb_Omb_Battlefield_node_star);
    star_init(&battlefield->star, star_node, &battlefield->player, bomb_om_battlefield_on_star_triggered, battlefield);
    star_collected_init(&battlefield->star_collected, engine, &battlefield->player, &battlefield->star);
}

#include "framework64/controller_mapping/n64.h"

void bomb_omb_battlefield_update(BombOmbBattlefield* battlefield) {
    // debug
    if (fw64_input_controller_button_pressed(battlefield->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_Z)) {
        battlefield->star_collected.state = STAR_COLLECTED_STATE_INACTIVE;
        star_reset(&battlefield->star);
        if (fw64_input_controller_button_down(battlefield->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_START)) {
            Vec3 debug_pos = {-24.21f, 57.63f, 52.0f};
            fw64_character_set_position(&battlefield->player.character, &debug_pos);
        } else {
            fw64_character_set_position(&battlefield->player.character, &battlefield->player.initial_pos);
        }
        
        battlefield->player.animation_controller.animation_state = FW64_INVALID_ANIMATION_ID; // temp
        player_set_character_control_enabled(&battlefield->player, 1);
        battlefield->star.triggered = 0;
    }

    player_update(&battlefield->player);
    fw64_headlight_update(&battlefield->headlight);
    fw64_skybox_update(&battlefield->skybox);
    star_update(&battlefield->star, battlefield->engine->time->time_delta);

    if (star_collected_is_active(&battlefield->star_collected)) {
        star_collected_update(&battlefield->star_collected, battlefield->engine->time->time_delta);
    }
}

void bomb_omb_battlefield_fixed_update(BombOmbBattlefield* battlefield) {
    fw64_character_environment_debug_info_reset(&battlefield->character_environment.debug_info);
    player_fixed_update(&battlefield->player);
}

void bomb_omb_battlefield_draw(BombOmbBattlefield* battlefield) {
    fw64_renderer_submit_renderpass(battlefield->engine->renderer, battlefield->skybox.renderpass);
    fw64_renderpass_set_camera(battlefield->scene_renderpass, &battlefield->player.camera);

    fw64Frustum view_frustum;
    fw64_camera_extract_frustum_planes(&battlefield->player.camera, &view_frustum);

    fw64_renderpass_begin(battlefield->scene_renderpass);
    fw64_scene_draw_frustrum(battlefield->scene, battlefield->scene_renderpass, &view_frustum, ~(FW64_layer_skybox | FW64_layer_player));

    // explicitly draw player.  They may disappear when under the influence of an uninterruptable animation that changes their position
    // The node position is not updated, but the animation is moving their geometry.  eg Ledge climb or ladder exit
    // TODO: this is probably not the best way to accomplish there
    fw64_renderpass_draw_skinned_mesh(battlefield->scene_renderpass, (fw64SkinnedMeshInstance*)battlefield->player.node->mesh_instance);
    fw64_renderpass_end(battlefield->scene_renderpass);

    fw64_renderer_submit_renderpass(battlefield->engine->renderer, battlefield->scene_renderpass);
}

static void bomb_om_battlefield_on_star_triggered(void* arg) {
    BombOmbBattlefield* battlefield = (BombOmbBattlefield*)arg;

    star_collected_start(&battlefield->star_collected, music_bank_bomb_omb_battlefield_music_courseclear);
}
