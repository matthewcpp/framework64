#include "game.h"
#include "assets/assets.h"
#include "assets/layers.h"
#include "assets/scene_Bomb_Omb_Battlefield.h"

#include "framework64/controller_mapping/n64.h"

void game_init(Game* game, fw64Engine* engine) {
    fw64Allocator* allocator = fw64_default_allocator();
    fw64Display* display = fw64_displays_get_primary(engine->displays);

    fw64_audio_load_musicbank_asset(engine->audio, engine->assets, FW64_ASSET_musicbank_music);

    game->engine = engine;

    fw64_collision_scene_manager_init(&game->collision_scene_manager, engine, display, allocator);
    fw64AssetId wireframe_scene_asset_id = 
    #ifdef FW64_PLATFORM_N64_LIBULTRA
        FW64_INVALID_ASSET_ID;
    #else
        FW64_ASSET_scene_Bomb_Omb_Battlefield_collision_wireframe;
    #endif
    fw64Scene* scene = fw64_collision_scene_manager_load_scene(&game->collision_scene_manager, FW64_ASSET_scene_Bomb_Omb_Battlefield, wireframe_scene_asset_id , FW64_layer_world);

    fw64Node* camera_node = fw64_scene_get_node(scene, FW64_scene_Bomb_Omb_Battlefield_node_camera);
    fw64_camera_init(&game->camera, camera_node, display);

    fw64_character_envionment_init(&game->character_environment);

    // Configure the "game specific player" which makes use of the third person character components
    // the initial orientation is set here, but in reality this may be set on a level by level basis
    fw64Node* player_node = fw64_scene_get_node(scene, FW64_scene_Bomb_Omb_Battlefield_node_player);
    quat_from_euler(&player_node->transform.rotation, 0, 270.0f, 0.0f);
    player_init(&game->player, engine, &game->character_environment, scene, player_node, &game->camera, allocator);

    game->collision_scene_manager.target = &game->player.node->transform;

    fw64_headlight_init(&game->headlight, game->collision_scene_manager.static_scene_renderpass, 0, &game->camera.node->transform);
    fw64Font* font = fw64_assets_load_font(engine->assets, FW64_ASSET_font_Consolas12, allocator);
    ui_init(&game->ui, engine, font, &game->player, allocator);

#ifndef COLLISION_TEST_NO_MUSIC
    fw64_audio_play_music(engine->audio, 0);
#endif
}

void game_update(Game* game){
    player_update(&game->player);
    fw64_headlight_update(&game->headlight);
    ui_update(&game->ui);
}

void game_fixed_update(Game* game) {
    player_fixed_update(&game->player);
}

void game_draw(Game* game) {
    fw64_collision_scene_manager_set_camera(&game->collision_scene_manager, &game->camera);
    fw64_collision_scene_manager_draw_scene(&game->collision_scene_manager);
    ui_draw(&game->ui);
}
