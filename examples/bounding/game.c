#include "game.h"
#include "assets/assets.h"
#include "assets/scene_Bounding_Example.h"
#include "assets/layers.h"

#include "framework64/matrix.h"
#include "framework64/controller_mapping/n64.h"
#include "framework64/util/renderpass_util.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

#define PENGUIN_SIZE_CHANGE_SPEED 0.15f
#define PENGUIN_MOVE_SPEED 85.0f
#define STICK_THRESHOLD 0.15f

static void setup_camera(Game* game);

static void penguin_init(Penguin* penguin, fw64Engine* engine, fw64Scene* scene);
static void penguin_update(Penguin* penguin);

static void ui_init(Ui* ui, fw64Engine* engine, Penguin* penguin, fw64Scene* scene);
static void ui_update(Ui* ui);
static void ui_draw(Ui* ui);

float stick_adjust[4];

void game_init(Game* game, fw64Engine* engine) {
    fw64Allocator* allocator =fw64_default_allocator();
    fw64Display* display = fw64_displays_get_primary(engine->displays);

    game->engine = engine;
    game->scene = fw64_assets_load_scene(game->engine->assets, FW64_ASSET_scene_Bounding_Example, allocator);
    setup_camera(game);
    penguin_init(&game->penguin, engine, game->scene);
    ui_init(&game->ui, engine, &game->penguin, game->scene);

    fw64Mesh* mesh = fw64_scene_load_mesh_asset(game->scene, FW64_ASSET_mesh_blue_cube_wire);
    fw64_debug_boxes_init(&game->debug_boxes, 1, game->scene, mesh, allocator);
    fw64Node* bounding_node = fw64_debug_boxes_add(&game->debug_boxes, game->penguin.node->mesh_instance);
    bounding_node->layer_mask = FW64_layer_lines;

    mat2_set_rotation(stick_adjust, M_PI / 2.0f);

    for (int i = 0; i < RENDERPASS_COUNT; i++) {
        game->renderpass[i] = fw64_renderpass_create(display, allocator);
        fw64_renderpass_set_camera(game->renderpass[i], &game->camera);
    }
}

void game_update(Game* game) {
    penguin_update(&game->penguin);
    ui_update(&game->ui);
    fw64_debug_boxes_update(&game->debug_boxes);
    (void)game;
}

static void draw_scene_layer(Game* game, fw64RenderPass* renderpass, fw64Frustum* frustum, uint32_t layer_mask) {
    fw64_renderpass_begin(renderpass);
    fw64_scene_draw_frustrum(game->scene, renderpass, frustum, layer_mask);
    fw64_renderpass_end(renderpass);

    fw64_renderer_submit_renderpass(game->engine->renderer, renderpass);
}

void game_draw(Game* game) {
    (void)game;

    fw64Frustum frustum;
    fw64_camera_extract_frustum_planes(&game->camera, &frustum);

    fw64_renderer_begin(game->engine->renderer, FW64_PRIMITIVE_MODE_TRIANGLES, FW64_CLEAR_FLAG_ALL);
    draw_scene_layer(game, game->renderpass[RENDERPASS_SCENE_TRIANGLES], &frustum, FW64_layer_triangles);
    ui_draw(&game->ui);
    fw64_renderer_end(game->engine->renderer, FW64_RENDERER_FLAG_NOSWAP);

    fw64_renderer_begin(game->engine->renderer, FW64_PRIMITIVE_MODE_LINES, FW64_CLEAR_FLAG_NONE);
    draw_scene_layer(game, game->renderpass[RENDERPASS_SCENE_LINES], &frustum, FW64_layer_lines);
    fw64_renderer_end(game->engine->renderer, FW64_RENDERER_FLAG_SWAP);
}

void penguin_init(Penguin* penguin, fw64Engine* engine, fw64Scene* scene) {
    penguin->node = fw64_scene_get_node(scene, FW64_scene_Bounding_Example_node_Penguin);
    fw64Mesh* mesh = fw64_scene_load_mesh_asset(scene, FW64_ASSET_mesh_penguin);
    Box mesh_bounding = fw64_mesh_get_bounding_box(mesh);
    penguin->engine = engine;
    penguin->scene = scene;
    penguin->active = NULL;
    fw64_scene_create_mesh_instance(scene, penguin->node, mesh);
    fw64_scene_create_box_collider(scene, penguin->node, &mesh_bounding);
}

void penguin_update(Penguin* penguin) {
    fw64Input* input = penguin->engine->input;
    fw64Node* node = penguin->node;

    Vec2 stick;
    fw64_input_controller_stick(penguin->engine->input, 0, &stick);
    
    if (stick.x >= STICK_THRESHOLD || stick.x <= -STICK_THRESHOLD  || stick.y >= STICK_THRESHOLD || stick.y <= -STICK_THRESHOLD) {
        mat2_transform_vec2(stick_adjust, &stick);
        float orientation = atan2f(stick.y, stick.x);
        quat_set_axis_angle(&node->transform.rotation, 0, 1, 0, orientation);

        Vec3 forward;
        fw64_transform_forward(&node->transform, &forward);
        vec3_scale(&forward, &forward, PENGUIN_MOVE_SPEED * penguin->engine->time->time_delta);
        vec3_add(&node->transform.position, &node->transform.position, &forward);
    }

    Vec3 scale_delta;
    vec3_set_all(&scale_delta, penguin->engine->time->time_delta * PENGUIN_SIZE_CHANGE_SPEED);
    if (fw64_input_controller_button_down(input, 0, FW64_N64_CONTROLLER_BUTTON_C_UP)) {
        vec3_add(&node->transform.scale, &node->transform.scale, &scale_delta);
    }

    if (fw64_input_controller_button_down(input, 0, FW64_N64_CONTROLLER_BUTTON_C_DOWN)) {
        vec3_negate(&scale_delta);
        vec3_add(&node->transform.scale, &node->transform.scale, &scale_delta);
    }

    fw64OverlapBoxQueryResult result;
    if (fw64_scene_overlap_box(penguin->scene, &penguin->node->collider->bounding, FW64_layer_cubes, &result)) {
        penguin->active = result.colliders[0]->node;
    } else {
        penguin->active = NULL;
    }

    fw64_node_update(node);
}

void ui_init(Ui* ui, fw64Engine* engine, Penguin* penguin, fw64Scene* scene) {
    ui->engine = engine;
    ui->penguin = penguin;
    ui->scene = scene;

    fw64Allocator* allocator = fw64_default_allocator();
    ui->font = fw64_assets_load_font(engine->assets, FW64_ASSET_font_Consolas12, allocator);
    ui->spritebatch = fw64_spritebatch_create(1, allocator);
    ui->renderpass = fw64_renderpass_create(fw64_displays_get_primary(engine->displays), allocator);
    fw64_renderpass_util_ortho2d(ui->renderpass);
}

static const char* node_names[4] = {"Mesh Bounding", "Green", "Yellow", "Transform"};

void ui_update(Ui* ui) {
    fw64_spritebatch_begin(ui->spritebatch);

    if (ui->penguin->active) {
        fw64_spritebatch_draw_string(ui->spritebatch, ui->font, node_names[ui->penguin->active->data], 10, 10);
    } else {
        fw64_spritebatch_draw_string(ui->spritebatch, ui->font, "None", 10, 10);
    }

    fw64_spritebatch_end(ui->spritebatch);
}

void ui_draw(Ui* ui) {
    fw64_renderpass_begin(ui->renderpass);
    fw64_renderpass_draw_sprite_batch(ui->renderpass, ui->spritebatch);
    fw64_renderpass_end(ui->renderpass);

    fw64_renderer_submit_renderpass(ui->engine->renderer, ui->renderpass);
}

void setup_camera(Game* game) {
    fw64_camera_init(&game->camera, fw64_displays_get_primary(game->engine->displays));

    game->camera.near = 10.0f;
    game->camera.far = 500.0f;
    fw64_camera_update_projection_matrix(&game->camera);

    fw64Node* node = fw64_scene_get_node(game->scene, FW64_scene_Bounding_Example_node_Camera);
    game->camera.transform.position = node->transform.position;

    Vec3 center = {0.0f, 0.0f, 0.0f}, up = {0.0f, 1.0f, 0.0f};
    fw64_transform_look_at(&game->camera.transform, &center, &up);

    fw64_camera_update_view_matrix(&game->camera);
}

