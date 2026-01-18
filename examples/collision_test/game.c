#include "game.h"
#include "assets/assets.h"
#include "assets/layers.h"
#include "assets/scene_Bomb_Omb_Battlefield.h"
#include "assets/scene_wire_primitives.h"

#include "framework64/controller_mapping/n64.h"

 #define BOMB_OMB_BATTLEFIELD_BUMP_ALLOCATOR_SIZE (475 * 1024)
 #define COLLISION_GEOMETRY_DEBUG_SIZE (25 * 1024)

void game_init(Game* game, fw64Engine* engine) {
    game->engine = engine;

    fw64Allocator* allocator = fw64_bump_allocator_init(&game->bump_allocator, BOMB_OMB_BATTLEFIELD_BUMP_ALLOCATOR_SIZE);
    
    bomb_omb_battlefield_init(&game->battlefield, engine, allocator);
    fw64Font* font = fw64_assets_load_font(engine->assets, FW64_ASSET_font_Consolas12, allocator);
    fw64DebugPrimitivesConfig config = {
        FW64_ASSET_scene_wire_primitives,
        FW64_scene_wire_primitives_node__capsule_stem,
        FW64_scene_wire_primitives_node__capsule_bottom,
        FW64_scene_wire_primitives_node__capsule_top,
        FW64_scene_wire_primitives_node__sphere,
        FW64_scene_wire_primitives_node__box,
        1.0f / 10.0f,
        1,
        1,
        1
    };

    fw64_character_debug_init(&game->character_debug, engine, COLLISION_GEOMETRY_DEBUG_SIZE, &config, allocator);
    fw64_character_debug_track(&game->character_debug, &game->battlefield.player.character, FW64_ASSET_file_Bomb_Omb_Battlefield_collision, &game->battlefield.player.camera);
    
    
    ui_init(&game->ui, engine, font, &game->battlefield.player, &game->character_debug.collision_geometry, &game->bump_allocator);
}

void game_update(Game* game){
    bomb_omb_battlefield_update(&game->battlefield);
    fw64_character_debug_update(&game->character_debug);
    ui_update(&game->ui);
}

void game_fixed_update(Game* game) {
    bomb_omb_battlefield_fixed_update(&game->battlefield);
    fw64_character_debug_fixed_update(&game->character_debug);
}

void game_draw(Game* game) {
    bomb_omb_battlefield_draw(&game->battlefield);
    ui_draw(&game->ui);
    fw64_character_debug_draw(&game->character_debug);
}
