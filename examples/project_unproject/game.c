#include "game.h"
#include "assets/assets.h"

#include "framework64/n64/controller_button.h"

void game_init(Game* game, fw64Engine* engine) {
    game->engine = engine;
    fw64Allocator* allocator = fw64_default_allocator();

    game->font = fw64_assets_load_font(engine->assets, FW64_ASSET_font_Consolas12, allocator);

    fw64_arcball_init(&game->arcball_camera, engine->input, fw64_displays_get_primary(engine->displays));
    
    fw64SceneInfo info;
    fw64_scene_info_init(&info);
    info.node_count = 1;
    info.mesh_count = 1;
    info.collider_count = 1;
    fw64_scene_init(&game->scene, &info, engine->assets, allocator);

    fw64Mesh* mesh = fw64_scene_load_mesh_asset(&game->scene, FW64_ASSET_mesh_blue_cube, 0);
    fw64Node* node = fw64_scene_get_node(&game->scene, 0);
    fw64Collider* collider = fw64_scene_get_collider(&game->scene, 0);
    fw64_node_set_mesh(node, mesh);
    fw64_node_set_box_collider(node, collider);
    fw64_node_update(node);

    fw64_arcball_set_initial(&game->arcball_camera, &collider->bounding);
}

void game_update(Game* game){
    fw64_arcball_update(&game->arcball_camera, game->engine->time->time_delta);
}

void game_draw(Game* game) {
    fw64_renderer_begin(game->engine->renderer, FW64_PRIMITIVE_MODE_TRIANGLES, FW64_RENDERER_FLAG_CLEAR);
    fw64_renderer_set_camera(game->engine->renderer, &game->arcball_camera.camera);
    fw64_scene_draw_all(&game->scene, game->engine->renderer);

    Vec3 min = {-1.0f, -1.0f, -1.0f}, max = {1.0f, 1.0f, 1.0f};
    IVec2 min_pos, max_pos;
    fw64_camera_world_to_viewport_pos(&game->arcball_camera.camera, &min, &min_pos);
    fw64_camera_world_to_viewport_pos(&game->arcball_camera.camera, &max, &max_pos);

    fw64_renderer_draw_text(game->engine->renderer, game->font, min_pos.x, min_pos.y, "min");
    fw64_renderer_draw_text(game->engine->renderer, game->font, max_pos.x, max_pos.y, "max");

    fw64_renderer_end(game->engine->renderer, FW64_RENDERER_FLAG_SWAP);
}
