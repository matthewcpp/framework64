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
    MESH_N64_BREW_LOGO,
    MESH_COUNT
} Meshes;

static const char* shading_mode_text[] = {
    "VertexColor",
    "VertexColorTextured",
    "Gouraud",
    "GouraudTextured",
    "UnlitTextured"
};

static void load_next_mesh(Game* game, int direction);
static void update_spritebatch(Game* game);

void game_init(Game* game, fw64Engine* engine) {
    fw64Allocator* allocator = fw64_default_allocator();
    fw64Display* display = fw64_displays_get_primary(engine->displays);

    game->engine = engine;
    game->font = fw64_assets_load_font(engine->assets, FW64_ASSET_font_Consolas12, allocator);
    game->button_sprite = fw64_texture_create_from_image(fw64_assets_load_image(engine->assets, FW64_ASSET_image_buttons, allocator), allocator);

    fw64SceneInfo info;
    fw64_scene_info_init(&info);
    info.node_count = 1;
    info.mesh_instance_count = 1;
    info.mesh_count = MESH_COUNT;

    fw64_scene_init(&game->scene, &info, engine->assets, allocator);
    fw64_scene_load_mesh_asset(&game->scene, FW64_ASSET_mesh_n64_logo);
    fw64_scene_load_mesh_asset(&game->scene, FW64_ASSET_mesh_controller_cube);
    fw64_scene_load_mesh_asset(&game->scene, FW64_ASSET_mesh_suzanne);
    fw64_scene_load_mesh_asset(&game->scene, FW64_ASSET_mesh_penguin);
    fw64_scene_load_mesh_asset(&game->scene, FW64_ASSET_mesh_n64_brew_logo);

    fw64_arcball_init(&game->arcball, engine->input, display);

    game->arcball.camera.near = 1.0f;
    game->arcball.camera.far = 1000.0f;
    fw64_camera_update_projection_matrix(&game->arcball.camera);

    game->spritebatch = fw64_spritebatch_create(1, allocator);
    game->renderpasses[RENDER_PASS_SCENE] = fw64_renderpass_create(display, allocator);
    game->renderpasses[RENDER_PASS_UI] = fw64_renderpass_create(display, allocator);
    fw64_renderpass_util_ortho2d(game->renderpasses[RENDER_PASS_UI]);

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

    if (fw64_scene_get_node_count(&game->scene) == 0) {
        fw64Node* node = fw64_scene_create_node(&game->scene);
        fw64_scene_create_mesh_instance(&game->scene, node, mesh);
    } else {
        fw64Node* node = fw64_scene_get_node(&game->scene, 0);
        fw64_mesh_instance_set_mesh(node->mesh_instance, mesh);
    }

    Box bounding_box = fw64_mesh_get_bounding_box(mesh);
    fw64_arcball_set_initial(&game->arcball, &bounding_box);

    update_spritebatch(game);
}

void game_update(Game* game) {
    if (fw64_input_controller_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_START)) {
        load_next_mesh(game, 1);
    }
    else if (fw64_input_controller_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_C_LEFT)) {
        load_next_mesh(game, -1);
    }

    fw64_arcball_update(&game->arcball, game->engine->time->time_delta);
}

void game_draw(Game* game) {
    fw64Renderer* renderer = game->engine->renderer;

    fw64_renderer_begin(renderer, FW64_PRIMITIVE_MODE_TRIANGLES, FW64_CLEAR_FLAG_ALL);

    fw64RenderPass* renderpass = game->renderpasses[RENDER_PASS_SCENE];
    fw64_renderpass_begin(renderpass);
    fw64_renderpass_set_camera(renderpass, &game->arcball.camera);
    fw64_scene_draw_all(&game->scene, renderpass);
    fw64_renderpass_end(renderpass);
    fw64_renderer_submit_renderpass(renderer, renderpass);

    renderpass = game->renderpasses[RENDER_PASS_UI];
    fw64_renderpass_begin(renderpass);
    fw64_renderpass_draw_sprite_batch(renderpass, game->spritebatch);
    fw64_renderpass_end(renderpass);
    fw64_renderer_submit_renderpass(renderer, renderpass);

    fw64_renderer_end(renderer, FW64_RENDERER_FLAG_SWAP);
}

void update_spritebatch(Game* game) {
    IVec2 screen_size = fw64_display_get_size(fw64_displays_get_primary(game->engine->displays));

    fw64_spritebatch_begin(game->spritebatch);

    int button_width = fw64_texture_slice_width(game->button_sprite);
    int draw_pos_x = 10;
    int draw_pos_y = screen_size.y - 40;

    fw64_spritebatch_draw_sprite_slice(game->spritebatch, game->button_sprite, 4, draw_pos_x, draw_pos_y);
    draw_pos_x += button_width + 4;

    fw64_spritebatch_draw_sprite_slice(game->spritebatch, game->button_sprite, 5, draw_pos_x, draw_pos_y);
    draw_pos_x += button_width + 4;

    fw64_spritebatch_draw_string(game->spritebatch, game->font, "Switch Model", draw_pos_x, draw_pos_y);

    IVec2 center_pos = fw64_text_util_center_string(game->font, shading_mode_text[game->current_mesh], &screen_size);
    fw64_spritebatch_draw_string(game->spritebatch, game->font, shading_mode_text[game->current_mesh], center_pos.x, 10);

    fw64_spritebatch_end(game->spritebatch);
}
