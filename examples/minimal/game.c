#include "game.h"
#include "assets.h"

#include <stddef.h>

fw64Texture* tex = NULL;

void game_init(Game* game, fw64Engine* engine) {
    game->engine = engine;
    fw64_camera_init(&game->camera);

    tex = fw64_assets_get_image(engine->assets, FW64_ASSET_sprite_ken);
    tex = fw64_assets_get_image(engine->assets, FW64_ASSET_sprite_ken);
}

void game_update(Game* game){
    (void)game;
}

void game_draw(Game* game) {
    int slice_height = fw64_texture_get_slice_height(tex);
    fw64_renderer_begin(game->engine->renderer, &game->camera, FW64_RENDERER_MODE_SPRITES, FW64_RENDERER_FLAG_CLEAR);
    fw64_renderer_draw_sprite_slice(game->engine->renderer, tex, 1, 10, 10);
    fw64_renderer_draw_sprite_slice(game->engine->renderer, tex, 7, 10, 10 + slice_height);
    fw64_renderer_end(game->engine->renderer, FW64_RENDERER_FLAG_SWAP);
}
