#include "game.h"
#include "assets/assets.h"
#include "assets/scene_CollisionTest.h"

#include "framework64/controller_mapping/n64.h"

void game_init(Game* game, fw64Engine* engine) {
    fw64Allocator* allocator = fw64_default_allocator();
    fw64Display* display = fw64_displays_get_primary(engine->displays);

    game->engine = engine;
    game->scene = fw64_assets_load_scene(engine->assets, FW64_ASSET_scene_CollisionTest, allocator);
    game->renderpass = fw64_renderpass_create(fw64_displays_get_primary(engine->displays), allocator);

    fw64Node* camera_node = fw64_scene_get_node(game->scene, FW64_scene_CollisionTest_node_Camera);
    fw64_camera_init(&game->camera, camera_node, display);
    fw64_renderpass_set_camera(game->renderpass, &game->camera);

    fw64Font* font = fw64_assets_load_font(engine->assets, FW64_ASSET_font_Consolas12, allocator);
    ui_init(&game->ui, engine, font, allocator);
}

void game_update(Game* game){
    ui_update(&game->ui);
}

void game_fixed_update(Game* game) {
    (void)game;
}

void game_draw(Game* game) {
    fw64Frustum view_frustum;
    fw64_camera_extract_frustum_planes(&game->camera, &view_frustum);

    fw64_renderpass_begin(game->renderpass);
    fw64_scene_draw_frustrum(game->scene, game->renderpass, &view_frustum, FW64_LAYER_MASK_ALL_LAYERS);
    fw64_renderer_submit_renderpass(game->engine->renderer, game->renderpass);

    ui_draw(&game->ui);
}
