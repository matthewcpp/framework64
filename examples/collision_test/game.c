#include "game.h"
#include "assets/assets.h"
#include "assets/layers.h"
#include "assets/scene_Bomb_Omb_Battlefield.h"

#include "framework64/controller_mapping/n64.h"

 #define BOMB_OMB_BATTLEFIELD_BUMP_ALLOCATOR_SIZE (500 * 1024)

void game_init(Game* game, fw64Engine* engine) {
    game->engine = engine;

    fw64Allocator* allocator = fw64_bump_allocator_init(&game->bump_allocator, BOMB_OMB_BATTLEFIELD_BUMP_ALLOCATOR_SIZE);
    
    bomb_omb_battlefield_init(&game->battlefield, engine, allocator);
    fw64Font* font = fw64_assets_load_font(engine->assets, FW64_ASSET_font_Consolas12, allocator);
    ui_init(&game->ui, engine, font, &game->battlefield.player, &game->bump_allocator);
}

void game_update(Game* game){
    bomb_omb_battlefield_update(&game->battlefield);
    ui_update(&game->ui);
}

void game_fixed_update(Game* game) {
    bomb_omb_battlefield_fixed_update(&game->battlefield);
}

void game_draw(Game* game) {
    bomb_omb_battlefield_draw(&game->battlefield);
    ui_draw(&game->ui);
}
