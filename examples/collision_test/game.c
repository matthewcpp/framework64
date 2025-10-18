#include "game.h"
#include "assets/assets.h"
#include "assets/layers.h"
#include "assets/scene_Bomb_Omb_Battlefield.h"

#include "framework64/controller_mapping/n64.h"

 #define BOMB_OMB_BATTLEFIELD_BUMP_ALLOCATOR_SIZE (475 * 1024)
 #define COLLISION_GEOMETRY_DEBUG_SIZE (25 * 1024)

void game_init(Game* game, fw64Engine* engine) {
    game->engine = engine;

    fw64Allocator* allocator = fw64_bump_allocator_init(&game->bump_allocator, BOMB_OMB_BATTLEFIELD_BUMP_ALLOCATOR_SIZE);
    
    bomb_omb_battlefield_init(&game->battlefield, engine, allocator);
    fw64Font* font = fw64_assets_load_font(engine->assets, FW64_ASSET_font_Consolas12, allocator);
    fw64_collision_geometry_debug_init(&game->collision_geometry_debug, engine, COLLISION_GEOMETRY_DEBUG_SIZE);
    game->collision_geometry_debug.layer_mask = FW64_COLLISION_GEOMETRY_DEBUG_LAYER_CELL_BOUNDINGS;
    fw64_collision_geometry_debug_load(&game->collision_geometry_debug, FW64_ASSET_file_Bomb_Omb_Battlefield_collision);
    fw64_collision_geometry_debug_track(&game->collision_geometry_debug, game->battlefield.scene->collision_geometry, &game->battlefield.player.node->transform, &game->battlefield.player.camera);
    ui_init(&game->ui, engine, font, &game->battlefield.player, &game->collision_geometry_debug, &game->bump_allocator);
}

void game_update(Game* game){
    bomb_omb_battlefield_update(&game->battlefield);
    fw64_collision_geometry_debug_update(&game->collision_geometry_debug);
    ui_update(&game->ui);
}

void game_fixed_update(Game* game) {
    bomb_omb_battlefield_fixed_update(&game->battlefield);
}

void game_draw(Game* game) {
    bomb_omb_battlefield_draw(&game->battlefield);
    ui_draw(&game->ui);
    fw64_collision_geometry_debug_draw(&game->collision_geometry_debug);
}
