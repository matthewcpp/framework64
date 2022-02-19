#include "game.h"
#include "assets.h"

#include "framework64/n64/controller_button.h"
#include "scene_splitscreen.h"

static void init_weapon(Game* game);
static void init_player(Game* player, int index, int node_id, float view_x, float view_y, float rot_y);

void game_init(Game* game, fw64Engine* engine) {
    game->engine = engine;

    fw64_renderer_set_clear_color(engine->renderer, 100, 149, 237);

    game->scene = fw64_scene_load(engine->assets, FW64_ASSET_scene_splitscreen, NULL);

    init_weapon(game);

    init_player(game, 0, FW64_scene_splitscreen_node_Player1, 0.0f, 0.0f, 270.0f);
    init_player(game, 1, FW64_scene_splitscreen_node_Player2, 0.5f, 0.0f, 90.0f);
    init_player(game, 2, FW64_scene_splitscreen_node_Player3, 0.0f, 0.5f, 180.0f);
    init_player(game, 3, FW64_scene_splitscreen_node_Player4, 0.5f, 0.5f, 0.0f);

    for (int i = 0; i < 4; i++)
        game->players[i].active = 1;
}

void game_update(Game* game){
    for (int i  = 0; i < 4; i++) {
        if (game->players[i].active)
            player_update(&game->players[i]);
    }
}

void game_draw(Game* game) {
    fw64_renderer_begin(game->engine->renderer, FW64_RENDERER_MODE_TRIANGLES, FW64_RENDERER_FLAG_CLEAR);
    
    for (int i  = 0; i < 4; i++) {
        if (game->players[i].active)
            player_draw_view(&game->players[i]);
    }

    fw64_renderer_end(game->engine->renderer, FW64_RENDERER_FLAG_SWAP);
}

void init_player(Game* game, int index, int node_id, float view_x, float view_y, float rot_y) {
    Player* player = &game->players[index];

    player_init(player, game->engine, game->scene, node_id, index);
    player_set_weapon(player, &game->us_ar33);
    player_set_viewport_rect(player, view_x, view_y, 0.5f, 0.5f);

    player->fps_camera.rotation.y = rot_y;
}

void init_weapon(Game* game) {
    Weapon* weapon = &game->us_ar33;

    weapon->mesh = fw64_mesh_load(game->engine->assets, FW64_ASSET_mesh_us_ar33, NULL);
    vec3_set(&weapon->position, 2.58f, -3.01f, -6.74f);
    vec3_set_all(&weapon->scale, 0.1f);
    quat_ident(&weapon->rotation);
}
