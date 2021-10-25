#include "game.h"

#include "assets.h"

#include "framework64/filesystem.h"
#include "framework64/n64/controller_button.h"

#include <string.h>

void game_init(Game* game, fw64Engine* engine) {
    game->engine = engine;
    fw64_camera_init(&game->camera);

    game->consolas = fw64_font_load(engine->assets, FW64_ASSET_font_Consolas12);
    game->scene = fw64_scene_load(engine->assets, FW64_ASSET_scene_n64);

    fw64Node* node0 = fw64_scene_get_node(game->scene, 0);
    fw64Node* node1 = fw64_scene_get_node(game->scene, 1);
    sprintf(game->str_buff, "%u %u %u %u " , 
    node0->mesh, node0->collider, node1->mesh, node1->collider);
}

void game_update(Game* game){

}

void game_draw(Game* game) {
    fw64_renderer_begin(game->engine->renderer, &game->camera, FW64_RENDERER_MODE_ORTHO2D, FW64_RENDERER_FLAG_CLEAR);
    fw64_renderer_draw_text(game->engine->renderer, game->consolas, 10, 10, game->str_buff);
    fw64_renderer_end(game->engine->renderer, FW64_RENDERER_FLAG_SWAP);
}
