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

    game->mode = GAME_MODE_SCENE;

    fw64SceneInfo info;
    fw64_scene_info_init(&info);
    info.mesh_count = 2;
    info.node_count = 2;
    info.mesh_instance_count = 1;
    fw64_scene_init(&game->scene, &info, engine->assets, allocator);

    fw64_scene_load_mesh_asset(&game->scene, FW64_ASSET_mesh_suzanne);
    fw64_scene_load_mesh_asset(&game->scene, FW64_ASSET_mesh_penguin);
    
    fw64Mesh* mesh = fw64_scene_get_mesh(&game->scene, 0);
    fw64Node* node = fw64_scene_create_node(&game->scene);
    fw64_scene_create_mesh_instance(&game->scene, node, mesh);

    fw64Node* camera_node = fw64_scene_create_node(&game->scene);
    fw64_camera_init(&game->camera, camera_node, display);
    game->camera.near = 5.0f;
    game->camera.far = 900.0f;
    fw64_camera_update_projection_matrix(&game->camera);

    fw64_arcball_init(&game->arcball, engine->input, &game->camera);
    Box mesh_bounding = fw64_mesh_get_bounding_box(mesh);
    fw64_arcball_set_initial(&game->arcball, &mesh_bounding);

    fw64_headlights_init(&game->headlights, allocator);

    ui_init(&game->ui, engine, &game->scene, game->renderpasses[RENDER_PASS_SCENE], &game->headlights, &game->arcball);
}

void game_update(Game* game){
    if (game->mode == GAME_MODE_SCENE) {
        fw64_arcball_update(&game->arcball, game->engine->time->time_delta);

        if (fw64_input_controller_button_released(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_START)) {
            game->mode = GAME_MODE_MENU;
            ui_activate(&game->ui);
        }
    } else {
        ui_update(&game->ui);

        if (!game->ui.is_active) {
            game->mode = GAME_MODE_SCENE;
        }
    }

    fw64_headlights_update(&game->headlights);
}

void game_draw(Game* game) {
    fw64_renderer_begin(game->engine->renderer, FW64_PRIMITIVE_MODE_TRIANGLES, FW64_CLEAR_FLAG_ALL);
    fw64RenderPass* renderpass = game->renderpasses[RENDER_PASS_SCENE];
    fw64_renderpass_set_camera(renderpass, game->arcball.camera);
    fw64_renderpass_begin(renderpass);
    fw64_scene_draw_all(&game->scene, renderpass);
    fw64_renderpass_end(renderpass);
    fw64_renderer_submit_renderpass(game->engine->renderer, renderpass);

    renderpass = game->renderpasses[RENDER_PASS_UI];
    fw64_renderpass_begin(renderpass);
    ui_draw(&game->ui, renderpass);
    fw64_renderpass_end(renderpass);
    fw64_renderer_submit_renderpass(game->engine->renderer, renderpass);

    fw64_renderer_end(game->engine->renderer, FW64_RENDERER_FLAG_SWAP);
}


