#include "game.h"

#include "assets/assets.h"
#include "assets/layers.h"
#include "assets/scene_billboard_example.h"

#include "framework64/controller_mapping/n64.h"

#include <math.h>

static void create_flame(Game* game);

#define FLAME_UPDATE_TIME 1.0f / 15.0f

void game_init(Game* game, fw64Engine* engine) {
    fw64Allocator* allocator = fw64_default_allocator();
    fw64Display* display = fw64_displays_get_primary(engine->displays);

    game->engine = engine;

    fw64_billboard_nodes_init(&game->billboard_nodes, allocator);

    game->scene = fw64_assets_load_scene(engine->assets, FW64_ASSET_scene_billboard_example, allocator);
    fw64Node* camera_node = fw64_scene_get_node(game->scene, FW64_scene_billboard_example_node_player);

    fw64_camera_init(&game->camera, camera_node, display);
    game->camera.near = 1.0f;
    game->camera.far = 300.0f;
    fw64_camera_update_projection_matrix(&game->camera);

    fw64_fps_camera_init(&game->fps, engine->input, &game->camera);
    game->fps.movement_speed = 70.0f;

    game->renderpass[RENDERPASS_DEPTH_ENABLED] = fw64_renderpass_create(display, allocator);
    fw64_renderpass_set_clear_color(game->renderpass[RENDERPASS_DEPTH_ENABLED], 0, 17, 51);
    game->renderpass[RENDERPASS_DEPTH_DISABLED] = fw64_renderpass_create(display, allocator);
    fw64_renderpass_set_depth_testing_enabled(game->renderpass[RENDERPASS_DEPTH_DISABLED], 0);

    create_flame(game);

    fw64Node* billboard_nodes[2];
    uint32_t node_count = fw64_scene_find_nodes_with_layer_mask(game->scene, FW64_layer_billboard, billboard_nodes, 2);
    for (uint32_t i = 0; i < node_count; i++) {
        fw64_billboard_nodes_create(&game->billboard_nodes, billboard_nodes[i], game->fps.camera, NULL);
    }
}

void create_flame(Game* game) {
    fw64Allocator* allocator = fw64_default_allocator();
    fw64Node* node = fw64_scene_get_node(game->scene, FW64_scene_billboard_example_node_flame);

    fw64Image* flame_image = fw64_assets_load_image_dma(game->engine->assets, FW64_ASSET_image_fire_sprite, allocator);
    fw64Material* material = fw64_material_collection_get_material(node->mesh_instance->materials, 0);
    fw64Texture* texture = fw64_texture_create_from_image(flame_image, allocator);
    fw64_texture_set_image(texture, flame_image);
    fw64_material_set_texture(material, texture, 0);

    game->flame.update_time_remaining = FLAME_UPDATE_TIME;
    game->flame.mesh_instance = node->mesh_instance;
}

static void flame_update(Flame* flame, float time_delta) {
    flame->update_time_remaining -= time_delta;

    if (flame->update_time_remaining <= 0.0f) {
        flame->update_time_remaining = FLAME_UPDATE_TIME;

        fw64Material* material = fw64_material_collection_get_material(flame->mesh_instance->materials, 0);
        fw64Texture* texture = fw64_material_get_texture(material);
        uint32_t frameCount = fw64_texture_hslices(texture) * fw64_texture_vslices(texture);
        uint32_t current_tex_frame = fw64_material_get_texture_frame(material);
        fw64_material_set_texture_frame(material, (current_tex_frame + 1) % frameCount);
    }
}

void game_update(Game* game){
    fw64_fps_camera_update(&game->fps, game->engine->time->time_delta);
    flame_update(&game->flame, game->engine->time->time_delta);
    fw64_billboard_nodes_update(&game->billboard_nodes);
}

void game_draw(Game* game) {
    fw64Renderer* renderer = game->engine->renderer;
    fw64Frustum frustum;
    fw64_camera_extract_frustum_planes(game->fps.camera, &frustum);

    fw64RenderPass* renderpass = game->renderpass[RENDERPASS_DEPTH_ENABLED];
    fw64_renderpass_begin(renderpass);
    fw64_renderpass_set_camera(renderpass, game->fps.camera);
    fw64_scene_draw_frustrum(game->scene, renderpass, &frustum, ~FW64_layer_flame);
    fw64_renderpass_end(renderpass);
    fw64_renderer_submit_renderpass(renderer, renderpass);

    renderpass = game->renderpass[RENDERPASS_DEPTH_DISABLED];
    fw64_renderpass_begin(renderpass);
    fw64_renderpass_set_camera(renderpass, game->fps.camera);
    fw64_scene_draw_frustrum(game->scene, renderpass, &frustum, FW64_layer_flame);
    fw64_renderpass_end(renderpass);
    fw64_renderer_submit_renderpass(renderer, renderpass);
}
