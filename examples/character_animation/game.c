#include "game.h"

#include "assets/assets.h"
#include "assets/catherine_animation.h"

#include "framework64/controller_mapping/n64.h"
#include "framework64/skinned_mesh_instance.h"

static void set_animation(Game* game, int animation);

void game_init(Game* game, fw64Engine* engine) {
    fw64Allocator* allocator = fw64_default_allocator();
    fw64Display* display = fw64_displays_get_primary(engine->displays);

    game->engine = engine;
    fw64_renderer_set_clear_color(engine->renderer, 100,149,237);

    fw64SceneInfo scene_info;
    fw64_scene_info_init(&scene_info);
    scene_info.node_count = 1;
    scene_info.skinned_mesh_count = 1;
    scene_info.skinned_mesh_instance_count = 1;

    fw64_scene_init(&game->scene, &scene_info, engine->assets, allocator);
    fw64Node* node = fw64_scene_get_node(&game->scene, 0);
    fw64SkinnedMesh* skinned_mesh = fw64_scene_load_skinned_mesh_asset(&game->scene, FW64_ASSET_skinnedmesh_catherine, 0);
    fw64SkinnedMeshInstance* skinned_mesh_instance = fw64_scene_get_skinned_mesh_instance(&game->scene, 0);
    fw64_skinned_mesh_instance_init(skinned_mesh_instance, node, skinned_mesh, 0, allocator);
    fw64_node_set_mesh_instance(node, (fw64MeshInstance*)skinned_mesh_instance);

    game->current_animation = 0;
    game->current_speed = 1.0f;

    
    fw64_arcball_init(&game->arcball, engine->input, display);

    fw64_arcball_set_initial(&game->arcball, &skinned_mesh_instance->mesh_instance.render_bounds);
    game->arcball.camera.near = 150.0f;
    game->arcball.camera.far = 750.0f;
    fw64_camera_update_projection_matrix(&game->arcball.camera);

    fw64_animation_controller_play(&skinned_mesh_instance->controller);

    game->renderpass = fw64_renderpass_create(display, allocator);
}


void game_update(Game* game) {
    fw64_arcball_update(&game->arcball, game->engine->time->time_delta);
    fw64_scene_update(&game->scene, game->engine->time->time_delta);

    if (fw64_input_controller_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_C_RIGHT)) {
        set_animation(game, game->current_animation + 1);
    }
    else if (fw64_input_controller_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_C_LEFT)) {
        set_animation(game, game->current_animation -1);
    }
}

void game_draw(Game* game) {
    fw64Renderer* renderer = game->engine->renderer;

    fw64_renderer_begin(renderer, FW64_PRIMITIVE_MODE_TRIANGLES, FW64_RENDERER_FLAG_CLEAR);
    fw64_renderpass_set_camera(game->renderpass,  &game->arcball.camera);
    fw64_renderpass_begin(game->renderpass);
    fw64_scene_draw_all(&game->scene, game->renderpass);
    fw64_renderpass_end(game->renderpass);
    fw64_renderer_submit_renderpass(renderer, game->renderpass);
    fw64_renderer_end(renderer, FW64_RENDERER_FLAG_SWAP);
}

static void set_animation(Game* game, int animation) {
    game->current_animation = animation;
    fw64SkinnedMeshInstance* skinned_mesh_instance = fw64_scene_get_skinned_mesh_instance(&game->scene, 0);
    fw64AnimationData* animation_data = skinned_mesh_instance->skinned_mesh->animation_data;

    if (game->current_animation >= (int)animation_data->animation_count) {
        game->current_animation = 0;
    } else if (game->current_animation < 0) {
        game->current_animation = animation_data->animation_count - 1;
    }

    fw64_animation_controller_set_animation(&skinned_mesh_instance->controller, game->current_animation);

    // note that the idle animation is sped up to reduce frame counts when exported from blender
    if (game->current_animation == catherine_animation_Idle) {
        skinned_mesh_instance->controller.speed = 0.1f;
    } else {
        skinned_mesh_instance->controller.speed = 1.0f;
    }
}
