#include "game.h"

#include "assets.h"

#include "framework64/filesystem.h"

#include <string.h>

void game_init(Game* game, System* system) {
    game->system = system;
    camera_init(&game->camera);

    game->consolas = assets_get_font(system->assets, ASSET_font_Consolas12);

    memset(game->str_buff, 0, 32);
    int handle = filesystem_open(ASSET_raw_sample);
    int size= filesystem_size(handle);
    filesystem_read(game->str_buff, 1, size, handle);
    filesystem_close(handle);
}

void game_update(Game* game, float time_delta){
}

void game_draw(Game* game) {
    renderer_begin(game->system->renderer, &game->camera, RENDERER_MODE_TRIANGLES, RENDERER_FLAG_CLEAR);
    renderer_draw_text(game->system->renderer, game->consolas, 10, 10, game->str_buff);
    renderer_end(game->system->renderer, RENDERER_FLAG_SWAP);
}
