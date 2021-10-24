#include "game.h"

#include "assets.h"

#include "framework64/n64/controller_button.h"

#define ENTITY_N64_LOGO 0
#define ENTITY_SUZANNE 1
#define ENTITY_PENGUIN 2

#define SWITCH_MODEL_TEXT "Switch Model"
#define ORBIT_CAMERA_TEXT "Orbit"
#define ZOOM_CAMERA_TEXT "ZOOM"

static void set_current_mesh(Game* game, int index);

void game_init(Game* game, fw64Engine* engine) {
    game->engine = engine;

    arcball_init(&game->arcball, engine->input);

    game->meshes[0] = fw64_mesh_load(engine->assets, FW64_ASSET_mesh_n64_logo);
    game->meshes[1] = fw64_mesh_load(engine->assets, FW64_ASSET_mesh_suzanne);
    game->meshes[2] = fw64_mesh_load(engine->assets, FW64_ASSET_mesh_penguin);

    fw64_node_init(&game->node);

    game->arcball.camera.near = 1.0f;
    game->arcball.camera.far = 1000.0f;
    fw64_camera_update_projection_matrix(&game->arcball.camera);

    game->consolas = fw64_font_load(engine->assets, FW64_ASSET_font_Consolas12);
    game->button_sprite = fw64_texture_create_from_image(fw64_image_load(engine->assets, FW64_ASSET_image_buttons));

    IVec2 text_measurement = fw64_font_measure_text(game->consolas, SWITCH_MODEL_TEXT);
    game->switch_model_text_width = text_measurement.x;

    set_current_mesh(game, 0);
}

static void set_current_mesh(Game* game, int index) {
    game->current_mesh = index;

    fw64Mesh* mesh = game->meshes[game->current_mesh];

    fw64_node_set_mesh(&game->node, mesh);

    Box bounding_box;
    fw64_mesh_get_bounding_box(mesh, &bounding_box);

    arcball_set_initial(&game->arcball, &bounding_box);
}

void game_update(Game* game) {
    if (fw64_input_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_C_RIGHT)) {
        set_current_mesh(game, ++game->current_mesh % MESH_COUNT);
    }

    if (fw64_input_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_C_LEFT)) {
        set_current_mesh(game, game->current_mesh > 0 ? game->current_mesh - 1 : MESH_COUNT - 1);
    }

    arcball_update(&game->arcball, game->engine->time->time_delta);
}

void game_draw(Game* game) {
    fw64Renderer* renderer = game->engine->renderer;
    fw64_renderer_begin(renderer, &game->arcball.camera, FW64_RENDERER_MODE_TRIANGLES, FW64_RENDERER_FLAG_CLEAR);
    fw64_renderer_draw_static_mesh(renderer, &game->node.transform, game->node.mesh);
    fw64_renderer_end(renderer, FW64_RENDERER_FLAG_NOSWAP);

    IVec2 screen_size;
    fw64_renderer_get_screen_size(renderer, &screen_size);

    int button_width = fw64_texture_slice_width(game->button_sprite);
    int draw_pos_x = 10;
    int draw_pos_y = 200;

    fw64_renderer_begin(renderer, &game->arcball.camera, FW64_RENDERER_MODE_ORTHO2D, FW64_RENDERER_FLAG_NOCLEAR);

    fw64_renderer_draw_sprite_slice(renderer, game->button_sprite, 15, draw_pos_x, draw_pos_y);
    draw_pos_x += button_width + 3;
    fw64_renderer_draw_text(renderer, game->consolas, draw_pos_x, draw_pos_y, ORBIT_CAMERA_TEXT);

    draw_pos_x = screen_size.x - game->switch_model_text_width - 3;
    fw64_renderer_draw_text(renderer, game->consolas, draw_pos_x, draw_pos_y, SWITCH_MODEL_TEXT);

    draw_pos_x -= button_width + 3;
    fw64_renderer_draw_sprite_slice(renderer, game->button_sprite, 5, draw_pos_x, draw_pos_y);

    draw_pos_x -= button_width + 3;
    fw64_renderer_draw_sprite_slice(renderer, game->button_sprite, 4, draw_pos_x, draw_pos_y);

    draw_pos_y = 220;
    draw_pos_x = 10;
    fw64_renderer_draw_sprite_slice(renderer, game->button_sprite, 6, draw_pos_x, draw_pos_y);

    draw_pos_x += button_width + 3;
    fw64_renderer_draw_sprite_slice(renderer, game->button_sprite, 7, draw_pos_x, draw_pos_y);

    draw_pos_x += button_width + 3;
    fw64_renderer_draw_text(renderer, game->consolas, draw_pos_x, draw_pos_y, ZOOM_CAMERA_TEXT);

    fw64_renderer_end(renderer, FW64_RENDERER_FLAG_SWAP);
}
