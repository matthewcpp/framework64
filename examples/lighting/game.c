#include "game.h"
#include "assets/assets.h"

#include "framework64/controller_mapping/n64.h"
#include "framework64/util/renderpass_util.h"

void game_init(Game* game, fw64Engine* engine) {
    fw64Allocator* allocator = fw64_default_allocator();
    fw64Display* display = fw64_displays_get_primary(engine->displays);

    game->engine = engine;

    game->renderpasses[RENDER_PASS_SCENE] = fw64_renderpass_create(display, allocator);
    game->renderpasses[RENDER_PASS_UI] = fw64_renderpass_create(display, allocator);
    fw64_renderpass_util_ortho2d(game->renderpasses[RENDER_PASS_UI]);

    fw64SceneInfo info;
    fw64_scene_info_init(&info);
    info.mesh_count = 1;
    info.node_count = 1;
    info.mesh_instance_count = 1;
    fw64_scene_init(&game->scene, &info, engine->assets, allocator);

    fw64Mesh* mesh = fw64_scene_load_mesh_asset(&game->scene, FW64_ASSET_mesh_suzanne);
    fw64Node* node = fw64_scene_create_node(&game->scene);
    fw64_scene_create_mesh_instance(&game->scene, node, mesh);

    fw64_arcball_init(&game->arcball, engine->input, display);
    Box mesh_bounding = fw64_mesh_get_bounding_box(mesh);
    fw64_arcball_set_initial(&game->arcball, &mesh_bounding);
    fw64_headlight_init(&game->headlight, game->renderpasses[RENDER_PASS_SCENE], 0, &game->arcball.camera);
}

void game_update(Game* game){
    fw64_arcball_update(&game->arcball, game->engine->time->time_delta);
    fw64_headlight_update(&game->headlight);
}

void game_draw(Game* game) {
    fw64_renderer_begin(game->engine->renderer, FW64_PRIMITIVE_MODE_TRIANGLES, FW64_CLEAR_FLAG_ALL);
    fw64RenderPass* renderpass = game->renderpasses[RENDER_PASS_SCENE];
    fw64_renderpass_set_camera(renderpass, &game->arcball.camera);
    fw64_renderpass_begin(renderpass);
    fw64_scene_draw_all(&game->scene, renderpass);
    fw64_renderpass_end(renderpass);
    fw64_renderer_submit_renderpass(game->engine->renderer, renderpass);

    fw64_renderer_end(game->engine->renderer, FW64_RENDERER_FLAG_SWAP);
}
