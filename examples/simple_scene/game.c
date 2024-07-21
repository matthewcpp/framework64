#include "game.h"

#include "framework64/util/renderpass_util.h"

#include "assets/assets.h"
#include "assets/scene_Playground.h"

void game_init(Game* game, fw64Engine* engine) {
    fw64Allocator* allocator = fw64_default_allocator();
    fw64Display* display = fw64_displays_get_primary(engine->displays);

    game->engine = engine;
    game->scene = fw64_assets_load_scene(game->engine->assets, FW64_ASSET_scene_Playground, allocator);
    game->renderpasses[RENDER_PASS_SCENE] = fw64_renderpass_create(display, allocator);
    game->renderpasses[RENDER_PASS_UI] = fw64_renderpass_create(display, allocator); 
    fw64_renderpass_util_ortho2d(game->renderpasses[RENDER_PASS_UI]);
    game->respawn_node = fw64_scene_get_node(game->scene, FW64_scene_Playground_node_Player_Start);

    player_init(&game->player, game->engine, game->scene, FW64_ASSET_mesh_penguin, NULL);
    vec3_set(&game->player.node->transform.scale, 0.01f, 0.01f, 0.01f);
    game->player.node->transform.position = game->respawn_node->transform.position;
    fw64_node_update(game->player.node);
    player_calculate_size(&game->player);

    chase_camera_init(&game->chase_cam, engine);
    game->chase_cam.target = &game->player.node->transform;

    ui_init(&game->ui, engine, &game->player);
}

void game_update(Game* game){
    player_update(&game->player);
    chase_camera_update(&game->chase_cam);

    if (game->player.node->transform.position.y < -10.0f) { // respawn player
        player_reset(&game->player, &game->respawn_node->transform.position);
    }

    ui_update(&game->ui);
}

void game_draw(Game* game) {
    fw64Frustum frustum;
    fw64_camera_extract_frustum_planes(&game->chase_cam.camera, &frustum);

    fw64_renderer_begin(game->engine->renderer, FW64_PRIMITIVE_MODE_TRIANGLES, FW64_RENDERER_FLAG_CLEAR);

    fw64RenderPass* renderpass = game->renderpasses[RENDER_PASS_SCENE];
    fw64_renderpass_begin(renderpass);
    fw64_renderpass_set_camera(renderpass, &game->chase_cam.camera);
    fw64_scene_draw_frustrum(game->scene, renderpass, &frustum, ~0U);
    fw64_renderpass_end(renderpass);
    fw64_renderer_submit_renderpass(game->engine->renderer, renderpass);

    renderpass = game->renderpasses[RENDER_PASS_UI];
    fw64_renderpass_begin(renderpass);
    fw64_renderpass_draw_sprite_batch(renderpass, game->ui.spritebatch);
    fw64_renderpass_end(renderpass);
    fw64_renderer_submit_renderpass(game->engine->renderer, renderpass);

    fw64_renderer_end(game->engine->renderer, FW64_RENDERER_FLAG_SWAP);
}
