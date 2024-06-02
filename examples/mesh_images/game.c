#include "game.h"
#include "assets/assets.h"

#include "framework64/controller_mapping/n64.h"

#include <stdio.h>

static void draw_ui_state(Game* game);
static void change_selection(Game* game, int direction);
static void change_palette(Game* game, int direction);

void game_init(Game* game, fw64Engine* engine) {
    game->engine = engine;

    game->font = fw64_assets_load_font(engine->assets, FW64_ASSET_font_Consolas, fw64_default_allocator());
    
    fw64Mesh* mesh = fw64_assets_load_mesh(engine->assets, FW64_ASSET_mesh_number_cube, fw64_default_allocator());
    fw64_node_init(&game->node);
    fw64_node_set_mesh(&game->node, mesh);

    Box bounding_box = fw64_mesh_get_bounding_box(mesh);
    fw64_arcball_init(&game->arcball, engine->input, fw64_displays_get_primary(engine->displays));
    fw64_arcball_set_initial(&game->arcball, &bounding_box);
    game->arcball.camera.near = 1.0f;
    game->arcball.camera.far = 20.0f;
    fw64_camera_update_projection_matrix(&game->arcball.camera);

    game->selected_material = 0;
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

void game_draw(Game* game) {
    fw64Renderer* renderer = game->engine->renderer;

    fw64_renderer_begin(renderer, FW64_PRIMITIVE_MODE_TRIANGLES, FW64_RENDERER_FLAG_CLEAR);
    fw64_renderer_set_camera(renderer, &game->arcball.camera);
    fw64_renderer_draw_static_mesh(renderer, &game->node.transform, game->node.mesh);
    draw_ui_state(game);
    fw64_renderer_end(renderer, FW64_RENDERER_FLAG_SWAP);
}

#define SELCTION_COUNT 6

void change_selection(Game* game, int direction) {
    game->selected_material += direction;

    if (game->selected_material >= SELCTION_COUNT)
        game->selected_material = 0;
    else if (game->selected_material < 0)
        game->selected_material = SELCTION_COUNT - 1;
}

void change_palette(Game* game, int direction) {
    fw64Material* material = fw64_mesh_get_material_for_primitive(game->node.mesh, game->selected_material);
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
}

static const char* names[SELCTION_COUNT] = {
    "Top",
    "Bottom",
    "Left",
    "Right",
    "Front",
    "Back"
};

void draw_ui_state(Game* game) {
    fw64Renderer* renderer = game->engine->renderer;
    int y_pos = 20;
    int font_size = fw64_font_size(game->font);
    char value_text[16];

    for (int i = 0; i < 6; i ++) {
        fw64Material* material = fw64_mesh_get_material_for_primitive(game->node.mesh, i);
        fw64Texture* texture = fw64_material_get_texture(material);

        sprintf(value_text, "%s: %d", names[i], fw64_texture_get_palette_index(texture));

        if (game->selected_material == i) {
            fw64_renderer_set_fill_color(renderer, 255, 255, 0, 255);
            fw64_renderer_draw_text(renderer, game->font, 20, y_pos, value_text);
            fw64_renderer_set_fill_color(renderer, 255, 255, 255, 255);
        }
        else {
            fw64_renderer_draw_text(renderer, game->font, 20, y_pos, value_text);
        }

        y_pos += font_size + 8;
    }
}
