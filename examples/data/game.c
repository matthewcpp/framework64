#include "game.h"

#include "assets.h"

#include "framework64/filesystem.h"

#include <string.h>

void game_init(Game* game, fw64Engine* engine) {
    game->engine = engine;
    fw64_camera_init(&game->camera);

    game->consolas = fw64_font_load(engine->assets, FW64_ASSET_font_Consolas12, NULL);

    fw64_renderer_set_anti_aliasing_enabled(engine->renderer, 0);

    memset(game->str_buff, 0, 32);
    int handle = fw64_filesystem_open(FW64_ASSET_raw_sample);
    int size= fw64_filesystem_size(handle);
    fw64_filesystem_read(game->str_buff, 1, size, handle);
    fw64_filesystem_close(handle);
}

void game_update(Game* game){
}

void game_draw(Game* game) {
    fw64_renderer_begin(game->engine->renderer, FW64_RENDERER_MODE_TRIANGLES, FW64_RENDERER_FLAG_CLEAR);
    fw64_renderer_set_camera(game->engine->renderer, &game->camera);
    fw64_renderer_draw_text(game->engine->renderer, game->consolas, 10, 10, game->str_buff);
    fw64_renderer_end(game->engine->renderer, FW64_RENDERER_FLAG_SWAP);
}
