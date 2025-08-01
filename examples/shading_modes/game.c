#include "game.h"

#include "assets/assets.h"

#include "framework64/controller_mapping/n64.h"
#include "framework64/util/renderpass_util.h"
#include "framework64/util/text_util.h"

typedef enum {
    MESH_N64_LOGO,
    MESH_CONTROLLER_CUBE,
    MESH_SUZANNE,
    MESH_PENGUIN,
    MESH_WIRE_CUBE,
    MESH_COUNT
} Meshes;

static const char* shading_mode_text[] = {
    "Unlit",
    "UnlitTextured",
    "Lit",
    "LitTextured",
    "Lines"
};

float mesh_scale [] = {
    0.1f,
    1.0f,
    1.0f,
    0.1f,
    1.0f
};

static void load_next_mesh(Game* game, int direction);
static void update_spritebatch(Game* game);

void game_init(Game* game, fw64Engine* engine) {
    fw64Allocator* allocator = fw64_default_allocator();
    fw64Display* display = fw64_displays_get_primary(engine->displays);

    game->engine = engine;
    game->font = fw64_assets_load_font(engine->assets, FW64_ASSET_font_Consolas12, allocator);
    game->mesh_node = NULL;

    fw64SceneInfo info;
    fw64_scene_info_init(&info);
    info.node_count = 2;
    info.mesh_instance_count = 1;
    info.mesh_count = MESH_COUNT;

    fw64_scene_init(&game->scene, &info, engine->assets, allocator);
    fw64_scene_load_mesh_asset(&game->scene, FW64_ASSET_mesh_n64_logo);
    fw64_scene_load_mesh_asset(&game->scene, FW64_ASSET_mesh_controller_cube);
    fw64_scene_load_mesh_asset(&game->scene, FW64_ASSET_mesh_suzanne);
    fw64_scene_load_mesh_asset(&game->scene, FW64_ASSET_mesh_penguin);
    fw64_scene_load_mesh_asset(&game->scene, FW64_ASSET_mesh_blue_cube_wire);

    fw64Node* camera_node = fw64_scene_create_node(&game->scene);
    fw64_camera_init(&game->camera, camera_node, display);
    game->camera.near = 1.0f;
    game->camera.far = 150.0f;
    fw64_camera_update_projection_matrix(&game->camera);

    fw64_arcball_init(&game->arcball, engine->input, &game->camera);

    game->spritebatch = fw64_spritebatch_create(1, allocator);
    game->renderpasses[RENDER_PASS_SCENE] = fw64_renderpass_create(display, allocator);
    game->renderpasses[RENDER_PASS_UI] = fw64_renderpass_create(display, allocator);
    fw64_renderpass_util_ortho2d(game->renderpasses[RENDER_PASS_UI]);

    fw64_headlight_init(&game->headlight, game->renderpasses[RENDER_PASS_SCENE], 0, &camera_node->transform);

    game->current_mesh = -1;
    load_next_mesh(game, 1);
}

static void load_next_mesh(Game* game, int direction) {
    game->current_mesh += direction;

    if (game->current_mesh >= MESH_COUNT) {
        game->current_mesh = 0;
    } else if (game->current_mesh < 0) {
        game->current_mesh = MESH_COUNT - 1;
    }

    fw64Mesh* mesh = fw64_scene_get_mesh(&game->scene, game->current_mesh);

    if (game->mesh_node == NULL) {
        game->mesh_node = fw64_scene_create_node(&game->scene);
    }

    vec3_set_all(&game->mesh_node->transform.scale, mesh_scale[game->current_mesh]);
    fw64_transform_update_matrix(&game->mesh_node->transform);
    fw64_renderpass_set_primitive_mode(game->renderpasses[RENDER_PASS_SCENE], fw64_mesh_primitive_get_mode(mesh, 0));

    if (game->mesh_node->mesh_instance == NULL) {
        fw64_scene_create_mesh_instance(&game->scene, game->mesh_node, mesh);
    } else {
        fw64_mesh_instance_set_mesh(game->mesh_node->mesh_instance, mesh);
    }

    fw64_arcball_set_initial(&game->arcball, &game->mesh_node->mesh_instance->render_bounds);

    update_spritebatch(game);
}

void game_update(Game* game) {
    if (fw64_input_controller_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_START)) {
        load_next_mesh(game, 1);
    }
    else if (fw64_input_controller_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_C_LEFT) ||
             fw64_input_controller_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_DPAD_LEFT))
    {
        load_next_mesh(game, -1);
    }
    else if (fw64_input_controller_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_C_RIGHT) ||
             fw64_input_controller_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_DPAD_RIGHT))
    {
        load_next_mesh(game, 1);
    }

    fw64_arcball_update(&game->arcball, game->engine->time->time_delta);
    fw64_headlight_update(&game->headlight);
}

void game_draw(Game* game) {
    fw64RenderPass* renderpass = game->renderpasses[RENDER_PASS_SCENE];
    fw64_renderpass_begin(renderpass);
    fw64_renderpass_set_camera(renderpass, &game->camera);
    fw64_scene_draw_all(&game->scene, renderpass);
    fw64_renderpass_end(renderpass);
    fw64_renderer_submit_renderpass(game->engine->renderer, renderpass);

    renderpass = game->renderpasses[RENDER_PASS_UI];
    fw64_renderpass_begin(renderpass);
    fw64_renderpass_draw_sprite_batch(renderpass, game->spritebatch);
    fw64_renderpass_end(renderpass);
    fw64_renderer_submit_renderpass(game->engine->renderer, renderpass);
}

void update_spritebatch(Game* game) {
    IVec2 screen_size = fw64_display_get_size(fw64_displays_get_primary(game->engine->displays));

    fw64_spritebatch_begin(game->spritebatch);

    IVec2 center_pos = fw64_text_util_center_string(game->font, shading_mode_text[game->current_mesh], &screen_size);
    fw64_spritebatch_draw_string(game->spritebatch, game->font, shading_mode_text[game->current_mesh], center_pos.x, 10);

    fw64_spritebatch_end(game->spritebatch);
}

void game_fixed_update(Game* game) {
    (void)game;
}
