#include "game.h"
#include "assets/assets.h"

#include <framework64/matrix.h>
#include <framework64/util/renderer_util.h>

#include "framework64/controller_mapping/n64.h"

static void init_scene(Game* game);
static void init_views(Game* game);
static void init_overlay(Game* game);

void game_init(Game* game, fw64Engine* engine) {
    game->engine = engine;
    game->font = fw64_assets_load_font(engine->assets, FW64_ASSET_font_Consolas12, fw64_default_allocator());

    init_scene(game);
    init_views(game);
    init_overlay(game);
}

void view_init(View* view, fw64Engine* engine, fw64Scene* scene, Vec2* viewport_pos, Vec2* viewport_size, fw64CameraProjectionMode projection_mode) {
    fw64Display* display = fw64_displays_get_primary(engine->displays);
    fw64_arcball_init(&view->arcball_camera, engine->input, display);
    view->arcball_camera.camera.projection_mode = projection_mode;

    fw64_camera_set_viewport_relative(&view->arcball_camera.camera, viewport_pos, viewport_size);
    fw64_camera_update_projection_matrix(&view->arcball_camera.camera);

    view->renderpass = fw64_renderpass_create(display, fw64_default_allocator());
    fw64_renderpass_set_clear_flags(view->renderpass, FW64_CLEAR_FLAG_ALL);
    fw64_renderpass_set_clear_color(view->renderpass, 25, 25, 25);

    Box bounding = {{-6.0f, -6.0f, -6.0f}, {6.0f, 6.0f, 6.0f}};
    fw64_arcball_set_initial(&view->arcball_camera, &bounding);

    fw64_renderpass_begin(view->renderpass);
    for (uint32_t i = 0; i < fw64_scene_get_node_count(scene); i++) {
        fw64Node* node = fw64_scene_get_node(scene, i);
        fw64_renderpass_draw_static_mesh(view->renderpass, node->mesh, &node->transform);
    }
    fw64_renderpass_end(view->renderpass);
}

void view_update(View* view, float time_delta) {
    fw64_arcball_update(&view->arcball_camera, time_delta);
    fw64_renderpass_set_camera(view->renderpass, &view->arcball_camera.camera);
}

void init_scene(Game* game) {
    fw64Scene* scene = &game->scene;
    fw64SceneInfo info;
    fw64_scene_info_init(&info);

    info.mesh_count = 1;
    info.node_count = 5;

    fw64_scene_init(scene, &info, game->engine->assets, fw64_default_allocator());
    fw64Mesh* mesh = fw64_scene_load_mesh_asset(scene, FW64_ASSET_mesh_blue_cube, 0);
    float x_pos = -3.0f;

    for (uint32_t i = 0; i < info.node_count; i++) {
        fw64Node* node = fw64_scene_get_node(scene, i);
        fw64_node_set_mesh(node, mesh);
        vec3_set(&node->transform.position, x_pos, 0.0f, 0.0f);
        fw64_node_update(node);
        x_pos += 3.0f;
    }
}

void init_views(Game* game) {
    fw64Display* display = fw64_displays_get_primary(game->engine->displays);

    IVec2 display_size = fw64_display_get_size(display);
    float display_aspect = (float)display_size.x / (float)display_size.y;
    float display_width = 0.46f;
    Vec2 viewport_size = {display_width, display_width/ display_aspect};

    Vec2 persp_pos = {0.02f, (1.0f - viewport_size.y) / 2.0f};
    view_init(&game->persp_view, game->engine, &game->scene, &persp_pos, &viewport_size, FW64_CAMERA_PROJECTION_PERSPECTIVE);

    Vec2 ortho_pos = {0.52f, (1.0f - viewport_size.y) / 2.0f};
    view_init(&game->ortho_view, game->engine, &game->scene, &ortho_pos, &viewport_size, FW64_CAMERA_PROJECTION_ORTHOGRAPHIC);
}

void init_overlay(Game* game) {
    fw64Display* display = fw64_displays_get_primary(game->engine->displays);
    IVec2 display_size = fw64_display_get_size(display);

    game->overlay.renderpass = fw64_renderpass_create(display, fw64_default_allocator());
    game->overlay.spritebatch = fw64_spritebatch_create(1, fw64_default_allocator());

    fw64_spritebatch_begin(game->overlay.spritebatch);
    fw64Viewport* viewport = &game->persp_view.arcball_camera.camera.viewport;
    fw64_spritebatch_draw_string(game->overlay.spritebatch, game->font, "persp", viewport->position.x, viewport->position.y);

    viewport = &game->ortho_view.arcball_camera.camera.viewport;
    fw64_spritebatch_draw_string(game->overlay.spritebatch, game->font, "ortho", viewport->position.x, viewport->position.y);

    fw64_spritebatch_end(game->overlay.spritebatch);

    fw64_renderpass_set_depth_testing_enabled(game->overlay.renderpass, 0);

    float projection_matrix[16];
    matrix_ortho2d(projection_matrix, 0, (float)display_size.x, (float)display_size.y, 0);
    fw64_renderpass_set_projection_matrix(game->overlay.renderpass, projection_matrix, NULL);

    fw64_renderpass_begin(game->overlay.renderpass);
    fw64_renderpass_draw_sprite_batch(game->overlay.renderpass, game->overlay.spritebatch);
    fw64_renderpass_end(game->overlay.renderpass);
}

void game_update(Game* game){
    view_update(&game->persp_view, game->engine->time->time_delta);
    view_update(&game->ortho_view, game->engine->time->time_delta);
}

void game_draw(Game* game) {
    fw64Renderer* renderer = game->engine->renderer;

    fw64_renderer_begin(renderer, FW64_PRIMITIVE_MODE_TRIANGLES, FW64_RENDERER_FLAG_NONE);
    fw64_renderer_submit_renderpass(renderer, game->persp_view.renderpass);
    fw64_renderer_submit_renderpass(renderer, game->ortho_view.renderpass);
    fw64_renderer_submit_renderpass(renderer, game->overlay.renderpass);
    fw64_renderer_end(game->engine->renderer, FW64_RENDERER_FLAG_SWAP);
}
