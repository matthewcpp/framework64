#include "game.h"
#include "assets/assets.h"
#include "assets/layers.h"
#include "assets/scene_CollisionTest.h"

#include "framework64/controller_mapping/n64.h"

void game_init(Game* game, fw64Engine* engine) {
    fw64Allocator* allocator = fw64_default_allocator();
    fw64Display* display = fw64_displays_get_primary(engine->displays);

    game->engine = engine;

    fw64_collision_scene_manager_init(&game->collision_scene_manager, engine, display, allocator);
    fw64Scene* scene = fw64_collision_scene_manager_load_scene(&game->collision_scene_manager, FW64_ASSET_scene_CollisionTest, FW64_ASSET_scene_CollisionTest_collision_wireframe, FW64_layer_world);

    fw64Node* camera_node = fw64_scene_get_node(scene, FW64_scene_CollisionTest_node_Camera);
    fw64_camera_init(&game->camera, camera_node, display);

    fw64_character_envionment_init(&game->character_environment);
    player_init(&game->player, engine, &game->character_environment, scene, fw64_scene_get_node(scene, FW64_scene_CollisionTest_node_Player));
    game->collision_scene_manager.target = &game->player.node->transform;
    fw64_third_person_camera_init(&game->third_person_cam, &game->player.node->transform, &game->camera);
    fw64_headlight_init(&game->headlight, game->collision_scene_manager.scene_renderpass, 0, &game->camera.node->transform);

    fw64Font* font = fw64_assets_load_font(engine->assets, FW64_ASSET_font_Consolas12, allocator);
    ui_init(&game->ui, engine, font, &game->player, allocator);
}

void game_update(Game* game){
    player_update(&game->player);
    fw64_third_person_camera_update(&game->third_person_cam);
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
