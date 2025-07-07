#include "game.h"
#include "assets/assets.h"
#include "assets/figure_animation.h"

#include "framework64/util/renderpass_util.h"
#include "framework64/controller_mapping/n64.h"

#include <stdio.h>
#include <string.h>

static const char* weapon_names[SELECTION_CATEGORY_COUNT] = {"Sword", "Mace"};
static const char* shield_names[SELECTION_CATEGORY_COUNT] = {"Kite", "Buckler"};

static void update_selection(Game* game);
static void update_spritebatch(Game* game);

void game_init(Game* game, fw64Engine* engine) {
    fw64Allocator* allocator = fw64_default_allocator();
    fw64Display* display = fw64_displays_get_primary(engine->displays);

    game->engine = engine;
    game->renderpass[RENDERPASS_SCENE] = fw64_renderpass_create(display, allocator);
    fw64_renderpass_set_light_enabled(game->renderpass[RENDERPASS_SCENE], 1, 1);
    Vec3 dir = {-0.57735f, 0.57735f, -0.57735f};
    fw64_renderpass_set_light_direction(game->renderpass[RENDERPASS_SCENE], 1, &dir);
    game->renderpass[RENDERPASS_UI] = fw64_renderpass_create(display, allocator);
    fw64_renderpass_util_ortho2d(game->renderpass[RENDERPASS_UI]);
    game->spritebatch = fw64_spritebatch_create(1, allocator);

    game->font = fw64_assets_load_font(engine->assets, FW64_ASSET_font_consolas12, allocator);

    memset(game->selections, 0, sizeof(game->selections));
    game->select_type = SELECT_WEAPON;

    fw64SceneInfo scene_info;
    fw64_scene_info_init(&scene_info);

    scene_info.mesh_count = SELECTION_CATEGORY_COUNT * 2;
    scene_info.mesh_instance_count = 2;
    scene_info.node_count = 5;
    scene_info.skinned_mesh_count = 1;
    scene_info.skinned_mesh_instance_count = 1;

    fw64_scene_init(&game->scene, &scene_info, engine->assets, allocator);
    fw64Node* root_node = fw64_scene_create_node(&game->scene);
    vec3_set_all(&root_node->transform.scale, 0.1f);

    fw64_camera_init(&game->camera, fw64_scene_create_node_with_parent(&game->scene, root_node), display);
    fw64_arcball_init(&game->arcball, engine->input, &game->camera);

    game->weapons[WEAPON_SWORD] = fw64_scene_load_mesh_asset(&game->scene, FW64_ASSET_mesh_sword);
    game->weapons[WEAPON_MACE] = fw64_scene_load_mesh_asset(&game->scene, FW64_ASSET_mesh_mace);
    game->shields[SHIELD_KITE] = fw64_scene_load_mesh_asset(&game->scene, FW64_ASSET_mesh_kite_shield);
    game->shields[SHIELD_BUCKLER] = fw64_scene_load_mesh_asset(&game->scene, FW64_ASSET_mesh_buckler);

    game->select_nodes[SELECT_WEAPON] = fw64_scene_create_mesh_instance(&game->scene, fw64_scene_create_node_with_parent(&game->scene, root_node), game->weapons[0])->node;
    quat_from_euler(&game->select_nodes[SELECT_WEAPON]->transform.rotation, 90.0f, 0.0f, 0.0f);
    vec3_set(&game->select_nodes[SELECT_WEAPON]->transform.position, 0.0f, 13.0f, 0.0f);
    game->select_nodes[SELECT_SHIELD] = fw64_scene_create_mesh_instance(&game->scene, fw64_scene_create_node_with_parent(&game->scene, root_node), game->shields[0])->node;
    quat_from_euler(&game->select_nodes[SELECT_SHIELD]->transform.rotation, 90.0f, 0.0f, -90.0f);
    vec3_set(&game->select_nodes[SELECT_SHIELD]->transform.position, -9.0f, 0.0f, 0.0f);

    fw64SkinnedMesh* skinned_mesh = fw64_scene_load_skinned_mesh_asset(&game->scene, FW64_ASSET_skinnedmesh_figure);
    fw64Node* character_node = fw64_scene_create_node_with_parent(&game->scene, root_node);
    game->character = fw64_scene_create_skinned_mesh_instance(&game->scene, character_node, skinned_mesh, figure_animation_Attack);
    fw64_transform_add_child(fw64_animation_controller_get_joint_transform(&game->character->controller, figure_joint_Hand_R), &game->select_nodes[SELECT_WEAPON]->transform);
    fw64_transform_add_child(fw64_animation_controller_get_joint_transform(&game->character->controller, figure_joint_Hand_L), &game->select_nodes[SELECT_SHIELD]->transform);

    fw64_animation_controller_play(&game->character->controller);

    fw64_node_update(root_node);

    Box mesh_bounding = fw64_mesh_get_bounding_box(game->character->mesh_instance.mesh), camera_bounding;
    matrix_transform_box(root_node->transform.world_matrix, &mesh_bounding, &camera_bounding);
    fw64_arcball_set_initial(&game->arcball, &camera_bounding);

    update_spritebatch(game);
}

void game_update(Game* game){
    update_selection(game);

    for (size_t i = 0; i < fw64_scene_get_skinned_mesh_instance_count(&game->scene); i++) {
        fw64_skinned_mesh_instance_update(fw64_scene_get_skinned_mesh_instance(&game->scene, i), game->engine->time->time_delta);
    }

    // ensure that the meshes attached to the skeleton are updated each frame
    for (size_t i = 0; i < SELECT_COUNT; i++) {
        fw64_mesh_instance_update(game->select_nodes[i]->mesh_instance);
    }

    fw64_arcball_update(&game->arcball, game->engine->time->time_delta);
    
    if (fw64_input_controller_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_START)) {
        if (!fw64_animation_controller_is_playing(&game->character->controller)) {
            fw64_animation_controller_play(&game->character->controller);
        } else {
            fw64_animation_controller_pause(&game->character->controller);
        }
    }
}

void game_draw(Game* game) {
    fw64RenderPass* renderpass = game->renderpass[RENDERPASS_SCENE];
    fw64_renderpass_begin(renderpass);
    fw64_renderpass_set_camera(renderpass, &game->camera);
    fw64_scene_draw_all(&game->scene, renderpass);
    fw64_renderpass_end(renderpass);

    fw64_renderer_begin(game->engine->renderer, FW64_PRIMITIVE_MODE_TRIANGLES, FW64_CLEAR_FLAG_ALL);
    fw64_renderer_submit_renderpass(game->engine->renderer, game->renderpass[RENDERPASS_SCENE]);
    fw64_renderer_submit_renderpass(game->engine->renderer, game->renderpass[RENDERPASS_UI]);
    fw64_renderer_end(game->engine->renderer, FW64_RENDERER_FLAG_SWAP);
}

void update_selection(Game* game) {
    int change_dir = 0;
    if (fw64_input_controller_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_DPAD_UP)) {
        change_dir = -1;
    } else if (fw64_input_controller_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_DPAD_DOWN)) {
        change_dir = 1;
    }

    if (change_dir != 0) {
        game->select_type += change_dir;
        if (game->select_type < 0) {
            game->select_type = SELECT_COUNT - 1;
        } else if (game->select_type >= SELECT_COUNT) {
            game->select_type = 0;
        }

        update_spritebatch(game);
        return;
    }

    if (fw64_input_controller_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_DPAD_LEFT)) {
        change_dir = -1;
    } else if (fw64_input_controller_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_DPAD_RIGHT)) {
        change_dir = 1;
    }

    if (change_dir != 0) {
        int new_index = game->selections[game->select_type] + change_dir;

        if (new_index < 0) {
            new_index = SELECTION_CATEGORY_COUNT - 1;
        } else if (new_index >= SELECTION_CATEGORY_COUNT) {
            new_index = 0;
        }

        game->selections[game->select_type] = new_index;

        if (game->select_type == SELECT_WEAPON) {
            fw64_mesh_instance_set_mesh(game->select_nodes[SELECT_WEAPON]->mesh_instance, game->weapons[new_index]);
        } else {
            fw64_mesh_instance_set_mesh(game->select_nodes[SELECT_SHIELD]->mesh_instance, game->shields[new_index]);
        }

        update_spritebatch(game);
    }
}

void update_spritebatch(Game* game) {
    char buffer[64];
    fw64_spritebatch_begin(game->spritebatch);
    if (game->select_type == SELECT_WEAPON){
        fw64_spritebatch_set_color(game->spritebatch, 255, 255, 0, 255);
    } else {
        fw64_spritebatch_set_color(game->spritebatch, 255, 255, 255, 255);
    }

    int y_pos = 10;
    sprintf(buffer, "Weapon: %s", weapon_names[game->selections[SELECT_WEAPON]]);
    fw64_spritebatch_draw_string(game->spritebatch, game->font, buffer, 10, y_pos);
    y_pos += fw64_font_line_height(game->font);

    if (game->select_type == SELECT_SHIELD){
        fw64_spritebatch_set_color(game->spritebatch, 255, 255, 0, 255);
    } else {
        fw64_spritebatch_set_color(game->spritebatch, 255, 255, 255, 255);
    }

    sprintf(buffer, "Shield: %s", shield_names[game->selections[SELECT_SHIELD]]);
    fw64_spritebatch_draw_string(game->spritebatch, game->font, buffer, 10, y_pos);

    fw64_spritebatch_end(game->spritebatch);

    fw64RenderPass* renderpass = game->renderpass[RENDERPASS_UI];
    fw64_renderpass_begin(renderpass);
    fw64_renderpass_draw_sprite_batch(renderpass, game->spritebatch);
    fw64_renderpass_end(renderpass);
}
