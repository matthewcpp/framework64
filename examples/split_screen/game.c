#include "game.h"
#include "assets/assets.h"

#include "framework64/controller_mapping/n64.h"
#include "framework64/matrix.h"

#include "assets/scene_Split_Screen.h"

static void init_weapon(Weapon* weapon, fw64Mesh* mesh);
static void init_player(Game* player, int index, int node_id, float view_x, float view_y, float rot_y);

void game_init(Game* game, fw64Engine* engine) {
    game->engine = engine;

    fw64_renderer_set_clear_color(engine->renderer, 100, 149, 237);

    game->scene = fw64_assets_load_scene(engine->assets, FW64_ASSET_scene_Split_Screen, fw64_default_allocator());

    init_weapon(&game->weapon, fw64_scene_load_mesh_asset(game->scene, FW64_ASSET_mesh_us_ar33));

    init_player(game, 0, FW64_scene_Split_Screen_node_Player1, 0.0f, 0.0f, 270.0f);
    init_player(game, 1, FW64_scene_Split_Screen_node_Player2, 0.5f, 0.0f, 90.0f);
    init_player(game, 2, FW64_scene_Split_Screen_node_Player3, 0.0f, 0.5f, 180.0f);
    init_player(game, 3, FW64_scene_Split_Screen_node_Player4, 0.5f, 0.5f, 0.0f);
}

void game_update(Game* game){
    for (int i  = 0; i < 4; i++) {
        player_update(&game->players[i]);
    }
}

void game_draw(Game* game) {
    fw64_renderer_begin(game->engine->renderer, FW64_PRIMITIVE_MODE_TRIANGLES, FW64_CLEAR_FLAG_ALL);
    for (int i  = 0; i < 4; i++) {
         player_draw_view(&game->players[i]);
    }
    
    for (int i  = 0; i < 4; i++) {
         player_draw_weapon(&game->players[i]);
    }

    fw64_renderer_end(game->engine->renderer, FW64_RENDERER_FLAG_SWAP);
}

void init_player(Game* game, int index, int node_id, float view_x, float view_y, float rot_y) {
    Player* player = &game->players[index];

    player_init(player, game->engine, game->scene, node_id, index);
    player_set_weapon(player, &game->weapon);
    player_set_viewport_rect(player, view_x, view_y, 0.5f, 0.5f);

    player->fps_camera.rotation.y = rot_y;
}

void init_weapon(Weapon* weapon, fw64Mesh* mesh) {
    weapon->mesh = mesh;

    Vec3 translation, scale;
    Quat rotation;

    vec3_set(&translation, 2.58f, -3.01f, -6.74f);
    vec3_set_all(&scale, 0.1f);
    quat_ident(&rotation);

    matrix_from_trs(weapon->view_matrix, &translation, &rotation, &scale);
    matrix_perspective(weapon->projection_matrix, 60.0f, 1.33f, 0.1f, 125.0f);
}
