#include "game.h"
#include "assets/assets.h"

#include "framework64/util/renderpass_util.h"
#include "framework64/util/texture_util.h"

#include "framework64/controller_mapping/n64.h"

#define BACKGROUND_MOVIE_COUNT 5

static const fw64AssetId backgroundMovies[BACKGROUND_MOVIE_COUNT] = {
    FW64_ASSET_file_washable1,
    FW64_ASSET_file_ninja2,
    FW64_ASSET_file_water1,
    FW64_ASSET_file_wires1,
    FW64_ASSET_file_robot10
};

void game_init(Game* game, fw64Engine* engine) {
    game->engine = engine;
    game->asset_index = 0;

    fw64Display* display = fw64_displays_get_primary(engine->displays);
    fw64Allocator* allocator = fw64_default_allocator();

    game->renderpass = fw64_renderpass_create(display, allocator);

    fw64SceneInfo scene_info;
    fw64_scene_info_init(&scene_info);
    scene_info.node_count = 2;
    scene_info.mesh_count = 1;
    scene_info.mesh_instance_count = 1; 
    scene_info.skinned_mesh_count = 1;
    scene_info.skinned_mesh_instance_count = 1;

    fw64_scene_init(&game->scene, &scene_info, engine->assets, allocator);

    fw64Node* camera_node = fw64_scene_create_node(&game->scene);
    vec3_set(&camera_node->transform.position, 0.1f, 11.0f, 52.3f);
    fw64_node_update(camera_node);
    fw64_camera_init(&game->camera, camera_node, display);
    game->camera.near = 5.0f;
    game->camera.far = 200.0f;
    fw64_camera_update_projection_matrix(&game->camera);
    //fw64_arcball_init(&game->arcball, engine->input, &game->camera);

    fw64SkinnedMesh* skinned_mesh = fw64_assets_load_skinned_mesh(game->engine->assets, FW64_ASSET_skinnedmesh_zombie, fw64_default_allocator());
    if (skinned_mesh) {
        fw64Node* node = fw64_scene_create_node(&game->scene);
        vec3_set_all(&node->transform.scale, 0.1f);
        fw64_node_update(node);

        fw64SkinnedMeshInstance* skinned_mesh_instance = fw64_scene_create_skinned_mesh_instance(&game->scene, node, skinned_mesh, 0);
        fw64_animation_controller_play(&skinned_mesh_instance->controller);
        skinned_mesh_instance->controller.speed = 2.5f;

        //fw64_arcball_set_initial(&game->arcball, &skinned_mesh_instance->mesh_instance.render_bounds);
    }

    fw64_fmv_open(engine->fmv, backgroundMovies[game->asset_index]);
    fw64_fmv_play(engine->fmv);

    fw64_audio_load_soundbank_asset(engine->audio, engine->assets, FW64_ASSET_file_moonlight_shadow1);
    game->music = fw64_audio_play_sound(engine->audio, 0);
}

void game_update(Game* game){
    fw64_scene_update(&game->scene, game->engine->time->time_delta);

    if (fw64_fmv_get_status(game->engine->fmv) == FW64_FMV_PLAYBACK_COMPLETE) {
        game->asset_index += 1;
        if (game->asset_index >= BACKGROUND_MOVIE_COUNT) {
            game->asset_index = 0;
        }

        fw64_fmv_close(game->engine->fmv);
        fw64_fmv_open(game->engine->fmv, backgroundMovies[game->asset_index]);
        fw64_fmv_play(game->engine->fmv);
    }

    //fw64_arcball_update(&game->arcball, 1.0f / 30.0f);
}

void game_draw(Game* game) {
    fw64_renderer_begin(game->engine->renderer, FW64_PRIMITIVE_MODE_TRIANGLES, FW64_CLEAR_FLAG_DEPTH );
    fw64_renderpass_begin(game->renderpass);
    fw64_renderpass_set_camera(game->renderpass, &game->camera);
    fw64_scene_draw_all(&game->scene, game->renderpass);
    fw64_renderpass_end(game->renderpass);
    fw64_renderer_submit_renderpass(game->engine->renderer, game->renderpass);
    fw64_renderer_end(game->engine->renderer, FW64_RENDERER_FLAG_SWAP);
}

