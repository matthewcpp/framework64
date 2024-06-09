#include "game.h"

#include "assets/assets.h"
#include "assets/layers.h"

#include "framework64/controller_mapping/n64.h"

#include <math.h>

static void create_ground_plane(Game* game, fw64Node* node);
static void create_flame(Game* game, fw64Node* node);
static void create_moon(Game* game, fw64Node* node);
static void create_firepit(Game* game, fw64Node* node);

void game_init(Game* game, fw64Engine* engine) {
    fw64Allocator* allocator = fw64_default_allocator();
    fw64Display* display = fw64_displays_get_primary(engine->displays);

    game->engine = engine;

    fw64_renderer_set_clear_color(engine->renderer, 0, 17, 51);
    billboard_nodes_init(&game->billboard_nodes, allocator);

    fw64_fps_camera_init(&game->fps, engine->input, fw64_displays_get_primary(engine->displays));
    game->fps.movement_speed = 70.0f;
    game->fps.camera.near = 1.0f;
    game->fps.camera.far = 300.0f;
    fw64_camera_update_projection_matrix(&game->fps.camera);
    vec3_set(&game->fps.camera.transform.position, 0.6f, 12.0f, 40.0f);

    game->renderpass[RENDERPASS_DEPTH_ENABLED] = fw64_renderpass_create(display, allocator);
    game->renderpass[RENDERPASS_DEPTH_DISABLED] = fw64_renderpass_create(display, allocator);
    fw64_renderpass_set_depth_testing_enabled(game->renderpass[RENDERPASS_DEPTH_DISABLED], 0);

    fw64SceneInfo scene_info;
    fw64_scene_info_init(&scene_info);
    scene_info.node_count = 3;
    scene_info.mesh_count = 1;

    fw64_scene_init(&game->scene, &scene_info, engine->assets, allocator);

    create_flame(game, fw64_scene_get_node(&game->scene, 0));
    create_ground_plane(game, fw64_scene_get_node(&game->scene, 1));
    create_moon(game, fw64_scene_get_node(&game->scene, 2));
}

void create_flame(Game* game, fw64Node* node) {
    flame_init(&game->flame, game->engine, node);
    vec3_set(&node->transform.position, 0, 11.0f, 0.0f);
    vec3_set(&node->transform.scale, 10.0f, 12.0f, 10.0f);
    fw64_node_update(node);
    billboard_nodes_create(&game->billboard_nodes, node, &game->fps.camera, NULL);
}

void create_firepit(Game* game, fw64Node* node) {
    fw64_node_init(node);
    fw64_node_set_mesh(node, fw64_assets_load_mesh(game->engine->assets, FW64_ASSET_mesh_campfire, fw64_default_allocator()));
    vec3_set(&node->transform.scale, 0.5f, 0.5f, 0.5f);
    fw64_node_update(node);
}

void create_moon(Game* game, fw64Node* node) {
    fw64_node_init(node);
    fw64_node_set_mesh(node, fw64_textured_quad_create(game->engine, FW64_ASSET_image_moon, fw64_default_allocator()));
    vec3_set(&node->transform.scale, 5.0f, 5.0f, 5.0f);
    vec3_set(&node->transform.position, -100.0f, 50.0f, -100.0f);
    fw64_node_update(node);
    billboard_nodes_create(&game->billboard_nodes, node, &game->fps.camera, NULL);
}

void create_ground_plane(Game* game, fw64Node* node) {
    fw64TexturedQuadParams params;
    fw64_textured_quad_params_init(&params);
    params.image_asset_id = FW64_ASSET_image_grass;
    params.max_s = 4.0f;
    params.max_t = 4.0f;

    fw64_node_init(node);
    fw64_node_set_mesh(node, fw64_textured_quad_create_with_params(game->engine, &params, fw64_default_allocator()));
    fw64Texture* texture = fw64_material_get_texture(fw64_mesh_get_material_for_primitive(node->mesh, 0));
    fw64_texture_set_wrap_mode(texture, FW64_TEXTURE_WRAP_REPEAT, FW64_TEXTURE_WRAP_REPEAT);
    quat_from_euler(&node->transform.rotation, 90.0f, 0.0f, 0.0f);
    vec3_set(&node->transform.scale, 100.0f, 100.0f, 100.0f);
    fw64_node_update(node);
}

void game_update(Game* game){
    fw64_fps_camera_update(&game->fps, game->engine->time->time_delta);
    flame_update(&game->flame, game->engine->time->time_delta);
    billboard_nodes_update(&game->billboard_nodes);
}

void game_draw(Game* game) {
    fw64Renderer* renderer = game->engine->renderer;
    fw64Frustum frustum;
    fw64_camera_extract_frustum_planes(&game->fps.camera, &frustum);

    fw64_renderer_begin(renderer, FW64_PRIMITIVE_MODE_TRIANGLES, FW64_RENDERER_FLAG_CLEAR);

    fw64RenderPass* renderpass = game->renderpass[RENDERPASS_DEPTH_ENABLED];
    fw64_renderpass_begin(renderpass);
    fw64_renderpass_set_camera(renderpass, &game->fps.camera);
    fw64_scene_draw_frustrum(&game->scene, renderpass, &frustum, ~FW64_layer_flame);
    fw64_renderpass_end(renderpass);
    fw64_renderer_submit_renderpass(renderer, renderpass);

    renderpass = game->renderpass[RENDERPASS_DEPTH_DISABLED];
    fw64_renderpass_begin(renderpass);
    fw64_renderpass_set_camera(renderpass, &game->fps.camera);
    fw64_scene_draw_frustrum(&game->scene, renderpass, &frustum, FW64_layer_flame);
    fw64_renderpass_end(renderpass);
    fw64_renderer_submit_renderpass(renderer, renderpass);

    fw64_renderer_end(renderer, FW64_RENDERER_FLAG_SWAP);
}
