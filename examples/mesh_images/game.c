#include "game.h"
#include "assets/assets.h"

#include "framework64/controller_mapping/n64.h"
#include "framework64/util/renderpass_util.h"

#include <stdio.h>

static void change_selection(Game* game, int direction);
static void change_palette(Game* game, int direction);
static void update_spritebatch(Game* game);

void game_init(Game* game, fw64Engine* engine) {
    fw64Allocator* allocator = fw64_default_allocator();
    fw64Display* display = fw64_displays_get_primary(engine->displays);

    game->engine = engine;
    game->font = fw64_assets_load_font(engine->assets, FW64_ASSET_font_Consolas, allocator);
    
    fw64SceneInfo info;
    fw64_scene_info_init(&info);
    info.node_count = 1;
    info.mesh_count = 1;
    info.mesh_instance_count = 1;
    fw64_scene_init(&game->scene, &info, engine->assets, allocator);
    fw64Node* node = fw64_scene_create_node(&game->scene);
    fw64Mesh* mesh = fw64_scene_load_mesh_asset(&game->scene, FW64_ASSET_mesh_number_cube);
    fw64_scene_create_mesh_instance(&game->scene, node, mesh);

    Box bounding_box = fw64_mesh_get_bounding_box(mesh);
    fw64_arcball_init(&game->arcball, engine->input, fw64_displays_get_primary(engine->displays));
    fw64_arcball_set_initial(&game->arcball, &bounding_box);
    game->arcball.camera.near = 1.0f;
    game->arcball.camera.far = 20.0f;
    fw64_camera_update_projection_matrix(&game->arcball.camera);

    game->renderpasses[RENDER_PASS_SCENE] = fw64_renderpass_create(display, allocator);
    game->renderpasses[RENDER_PASS_UI] = fw64_renderpass_create(display, allocator);
    fw64_renderpass_util_ortho2d(game->renderpasses[RENDER_PASS_UI]);

    game->spritebatch = fw64_spritebatch_create(1, allocator);
    game->selected_material = 3;
    change_selection(game, 1);
}

void game_update(Game* game){
    fw64Input* input = game->engine->input;
    fw64_arcball_update(&game->arcball, game->engine->time->time_delta);

    if (fw64_input_controller_button_pressed(input, 0, FW64_N64_CONTROLLER_BUTTON_DPAD_UP) | fw64_input_controller_button_pressed(input, 0, FW64_N64_CONTROLLER_BUTTON_C_UP))
        change_selection(game, -1);
    else if (fw64_input_controller_button_pressed(input, 0, FW64_N64_CONTROLLER_BUTTON_DPAD_DOWN) | fw64_input_controller_button_pressed(input, 0, FW64_N64_CONTROLLER_BUTTON_C_DOWN))
        change_selection(game, 1);

    if (fw64_input_controller_button_pressed(input, 0, FW64_N64_CONTROLLER_BUTTON_DPAD_RIGHT) | fw64_input_controller_button_pressed(input, 0, FW64_N64_CONTROLLER_BUTTON_C_RIGHT))
        change_palette(game, 1);
    else if (fw64_input_controller_button_pressed(input, 0, FW64_N64_CONTROLLER_BUTTON_DPAD_LEFT) | fw64_input_controller_button_pressed(input, 0, FW64_N64_CONTROLLER_BUTTON_C_LEFT))
        change_palette(game, -1);
}

void change_palette(Game* game, int direction) {
    fw64Mesh* mesh = fw64_scene_get_mesh(&game->scene, 0);
    fw64Material* material = fw64_mesh_get_material_for_primitive(mesh, game->selected_material);
    fw64Texture* texture = fw64_material_get_texture(material);
    fw64Image* image = fw64_texture_get_image(texture);
    int palette_count = fw64_image_get_palette_count(image);
    int selected_palette = fw64_texture_get_palette_index(texture);

    selected_palette += direction;

    if (selected_palette >= palette_count)
        selected_palette = 0;
    else if (selected_palette < 0)
        selected_palette = palette_count - 1;

    fw64_texture_set_palette_index(texture, selected_palette);

    update_spritebatch(game);
}

void game_draw(Game* game) {
    fw64Renderer* renderer = game->engine->renderer;

    fw64_renderer_begin(renderer, FW64_PRIMITIVE_MODE_TRIANGLES, FW64_RENDERER_FLAG_CLEAR);

    fw64RenderPass* renderpass = game->renderpasses[RENDER_PASS_SCENE];
    fw64_renderpass_set_camera(renderpass, &game->arcball.camera);
    fw64_renderpass_begin(renderpass);
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

#define SELCTION_COUNT 6

void change_selection(Game* game, int direction) {
    game->selected_material += direction;

    if (game->selected_material >= SELCTION_COUNT)
        game->selected_material = 0;
    else if (game->selected_material < 0)
        game->selected_material = SELCTION_COUNT - 1;

    update_spritebatch(game);
}

static const char* names[SELCTION_COUNT] = {
    "Top",
    "Bottom",
    "Left",
    "Right",
    "Front",
    "Back"
};

void update_spritebatch(Game* game) {
    int y_pos = 20;
    int line_height = fw64_font_line_height(game->font);
    char value_text[16];

    fw64_spritebatch_begin(game->spritebatch);
    fw64Mesh* mesh = fw64_scene_get_mesh(&game->scene, 0);

    for (int i = 0; i < 6; i ++) {
        fw64Material* material = fw64_mesh_get_material_for_primitive(mesh, i);
        fw64Texture* texture = fw64_material_get_texture(material);

        sprintf(value_text, "%s: %d", names[i], fw64_texture_get_palette_index(texture));

        if (game->selected_material == i) {
            fw64_spritebatch_set_color(game->spritebatch, 255, 255, 0, 255);
            fw64_spritebatch_draw_string(game->spritebatch, game->font, value_text, 20, y_pos);
            fw64_spritebatch_set_color(game->spritebatch, 255, 255, 255, 255);
        }
        else {
            fw64_spritebatch_draw_string(game->spritebatch, game->font, value_text, 20, y_pos);
        }

        y_pos += line_height;
    }

    fw64_spritebatch_end(game->spritebatch);
}
