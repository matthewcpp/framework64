#include "game.h"
#include "assets/assets.h"

#include "framework64/controller_mapping/n64.h"

#include "framework64/util/texture_util.h"

// https://github.com/matthewcpp/n64brew-gamejam-2021/blob/main/src/player_palette.c

#define HAIR_PRIMITIVE_INDICES_COUNT 3
#define SKIN_PRIMITIVE_INDICES_COUNT 5
#define CLOTHES_PRIMITIVE_INDICES_COUNT 2
#define TEXTURE_PRIMITIVE_INDICES_COUNT 4

static int hair_primitive_indices[HAIR_PRIMITIVE_INDICES_COUNT] = {0, 1, 2};
static int skin_primitive_indices[SKIN_PRIMITIVE_INDICES_COUNT] = {3, 4, 5, 6, 7};
static int clothes_primitive_indices[CLOTHES_PRIMITIVE_INDICES_COUNT] = {12, 13};
static int texture_indices[TEXTURE_PRIMITIVE_INDICES_COUNT] = {17, 24, 25, 26};
static int texture_assets[TEXTURE_PRIMITIVE_INDICES_COUNT] = {FW64_ASSET_image_catherine_alt_face, FW64_ASSET_image_catherine_alt_pants, FW64_ASSET_image_catherine_alt_chest, FW64_ASSET_image_catherine_alt_back};

static fw64ColorRGBA8 hair_color = {52, 209, 237, 255};
static fw64ColorRGBA8 skin_color = {133, 97, 69, 255};
static fw64ColorRGBA8 clothes_color = {130, 26, 26, 255};

void alternate_palette_init(Game* game) {
    fw64Allocator* allocator = fw64_default_allocator();
    fw64SkinnedMesh* skinned_mesh = fw64_scene_get_skinned_mesh(&game->scene, 0);
    fw64_material_collection_init_clone(&game->alternate_palette, fw64_mesh_get_material_collection(skinned_mesh->mesh), allocator);

    for (int i = 0; i < HAIR_PRIMITIVE_INDICES_COUNT; i++) {
        fw64Material* hair_material = fw64_material_collection_get_material(&game->alternate_palette, hair_primitive_indices[i]);
        fw64_material_set_color(hair_material, hair_color);
    }

    for (int i = 0; i < SKIN_PRIMITIVE_INDICES_COUNT; i++) {
        fw64Material* skin_material = fw64_material_collection_get_material(&game->alternate_palette, skin_primitive_indices[i]);
        fw64_material_set_color(skin_material, skin_color);
    }

    for (int i = 0; i < CLOTHES_PRIMITIVE_INDICES_COUNT; i++) {
        fw64Material* hair_material = fw64_material_collection_get_material(&game->alternate_palette, clothes_primitive_indices[i]);
        fw64_material_set_color(hair_material, clothes_color);
    }

    for (int i = 0; i < TEXTURE_PRIMITIVE_INDICES_COUNT; i++) {
        fw64Texture* texture = fw64_texture_util_create_from_loaded_image(game->engine->assets, texture_assets[i], allocator);
        fw64Material* textured_material = fw64_material_collection_get_material(&game->alternate_palette, texture_indices[i]);
        fw64_material_set_texture(textured_material, texture, 0);
    }
}

static void toggle_material_collection(Game* game, int mesh_index) {
    fw64SkinnedMeshInstance* instance = fw64_scene_get_skinned_mesh_instance(&game->scene, mesh_index);
    fw64MaterialCollection* collection = instance->mesh_instance.materials == &game->alternate_palette ? 
        fw64_mesh_get_material_collection(instance->skinned_mesh->mesh) : &game->alternate_palette;
    
    fw64_skinned_mesh_instance_set_material_collection(instance, collection);
}

static void create_skinned_mesh_instance(Game* game, float x_pos);
static void setup_camera(Game* game);

void game_init(Game* game, fw64Engine* engine) {
    fw64Allocator* allocator = fw64_default_allocator();
    fw64Display* display = fw64_displays_get_primary(engine->displays);
    game->engine = engine;

    fw64SceneInfo scene_info;
    fw64_scene_info_init(&scene_info);
    scene_info.node_count = 3;
    scene_info.skinned_mesh_count = 1;
    scene_info.skinned_mesh_instance_count = 2;

    fw64_scene_init(&game->scene, &scene_info, engine->assets, allocator);
    fw64_scene_load_skinned_mesh_asset(&game->scene, FW64_ASSET_skinnedmesh_catherine);
    create_skinned_mesh_instance(game, -6.0f);
    create_skinned_mesh_instance(game, 6.0f);

    setup_camera(game);

    game->renderpass = fw64_renderpass_create(display, allocator);
    fw64_renderpass_set_light_enabled(game->renderpass, 0, 1);
    fw64_headlight_init(&game->headlight, game->renderpass, 0, &game->camera.node->transform);

    alternate_palette_init(game);
    toggle_material_collection(game, 1);
}

void create_skinned_mesh_instance(Game* game, float x_pos) {
    fw64SkinnedMesh* skinned_mesh = fw64_scene_get_skinned_mesh(&game->scene, 0);
    fw64Node* node = fw64_scene_create_node(&game->scene);
    vec3_set(&node->transform.position, x_pos, 0.0f, 0.0f);
    vec3_set_all(&node->transform.scale, 0.1f);
    fw64_node_update(node);
    fw64SkinnedMeshInstance* instance = fw64_scene_create_skinned_mesh_instance(&game->scene, node, skinned_mesh, 0);
    fw64_animation_controller_play(&instance->controller);
}

void setup_camera(Game* game) {
    fw64SkinnedMeshInstance* instance = fw64_scene_get_skinned_mesh_instance(&game->scene, 0);
    Box bounding = instance->mesh_instance.render_bounds;

    for (uint32_t i = 1; i < fw64_scene_get_skinned_mesh_instance_count(&game->scene); i++) {
        instance = fw64_scene_get_skinned_mesh_instance(&game->scene, i);
        box_encapsulate_box(&bounding, &instance->mesh_instance.render_bounds);
    }

    fw64Node* camera_node = fw64_scene_create_node(&game->scene);
    fw64_camera_init(&game->camera, camera_node, fw64_displays_get_primary(game->engine->displays));
    fw64_arcball_init(&game->arcball, game->engine->input, &game->camera);
    fw64_arcball_set_initial(&game->arcball, &bounding);
}

void game_update(Game* game){
    for (size_t i = 0; i < fw64_scene_get_skinned_mesh_instance_count(&game->scene); i++) {
        fw64_skinned_mesh_instance_update(fw64_scene_get_skinned_mesh_instance(&game->scene, i), game->engine->time->time_delta);
    }
    
    fw64_arcball_update(&game->arcball, game->engine->time->time_delta);
    fw64_headlight_update(&game->headlight);

    if (fw64_input_controller_button_released(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_DPAD_UP) || 
        fw64_input_controller_button_released(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_DPAD_DOWN))
    {
        toggle_material_collection(game, 0);
    }

    if (fw64_input_controller_button_released(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_C_UP) || 
        fw64_input_controller_button_released(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_C_DOWN))
    {
        toggle_material_collection(game, 1);
    }
}

void game_draw(Game* game) {
    fw64_renderer_begin(game->engine->renderer, FW64_PRIMITIVE_MODE_TRIANGLES, FW64_CLEAR_FLAG_ALL);
    fw64_renderpass_begin(game->renderpass);
    fw64_renderpass_set_camera(game->renderpass, &game->camera);
    fw64_scene_draw_all(&game->scene, game->renderpass);
    fw64_renderpass_end(game->renderpass);
    fw64_renderer_submit_renderpass(game->engine->renderer, game->renderpass);
    fw64_renderer_end(game->engine->renderer, FW64_RENDERER_FLAG_SWAP);
}
