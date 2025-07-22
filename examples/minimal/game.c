#include "game.h"
#include "assets/assets.h"

#include "framework64/controller_mapping/n64.h"

void game_init(Game* game, fw64Engine* engine) {
    game->engine = engine;
    game->renderpass = fw64_renderpass_create(fw64_displays_get_primary(engine->displays), fw64_default_allocator());
    fw64_renderpass_set_clear_color(game->renderpass, 0, 200, 0);
}

void game_update(Game* game){
    (void)game;
}

void game_fixed_update(Game* game){
    (void)game;
}

void game_draw(Game* game) {
    fw64_renderer_submit_renderpass(game->engine->renderer, game->renderpass);
}
