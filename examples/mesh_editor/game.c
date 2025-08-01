#include "game.h"

#include "framework64/util/mesh_editor.h"

#include "assets/assets.h"

#include "framework64/controller_mapping/n64.h"

#define TEXTURE_TILE_COUNT 16

static void set_tile_tex_coords(Game* game) {
    float x = (float)(game->tile_index % 4) * 0.25f;
    float y = (float)(game->tile_index / 4) * 0.25f;

    fw64MeshEditor mesh_editor;
    fw64Mesh* mesh = fw64_scene_get_mesh(&game->scene, 0);
    fw64_mesh_editor_init(&mesh_editor, mesh);
    fw64_mesh_editor_set_active_primitive(&mesh_editor, 0);

    fw64_mesh_editor_set_vertex_texcoords_f(&mesh_editor, 0, x, y + 0.25f);
    fw64_mesh_editor_set_vertex_texcoords_f(&mesh_editor, 1, x + 0.25f, y + 0.25f);
    fw64_mesh_editor_set_vertex_texcoords_f(&mesh_editor, 2, x + 0.25f, y);
    fw64_mesh_editor_set_vertex_texcoords_f(&mesh_editor, 3, x, y);

    fw64_mesh_editor_commit(&mesh_editor);
}

static void set_next_tile(Game* game, int direction) {
    game->tile_index += direction;

    if (game->tile_index >= TEXTURE_TILE_COUNT) {
        game->tile_index = 0;
    } else if (game->tile_index < 0) {
        game->tile_index = TEXTURE_TILE_COUNT - 1;
    }

    set_tile_tex_coords(game);
}

void game_init(Game* game, fw64Engine* engine) {
    fw64Allocator* allocator = fw64_default_allocator();
    fw64Display* display = fw64_displays_get_primary(engine->displays);

    game->engine = engine;
    game->tile_index = -1;
    game->renderpass = fw64_renderpass_create(display, allocator);

    fw64SceneInfo info;
    fw64_scene_info_init(&info);
    info.mesh_count = 1;
    info.mesh_instance_count = 1;
    info.node_count = 2;
    fw64_scene_init(&game->scene, &info, engine->assets, allocator);

    fw64Node* camera_node = fw64_scene_create_node(&game->scene);
    vec3_set(&camera_node->transform.position, 0.0f, 0.0f, 5.0f);
    fw64_node_update(camera_node);
    fw64Camera camera;
    fw64_camera_init(&camera, camera_node, display);
    fw64_renderpass_set_camera(game->renderpass, &camera);

    fw64Node* node = fw64_scene_create_node(&game->scene);
    fw64Mesh* mesh = fw64_scene_load_mesh_asset(&game->scene, FW64_ASSET_mesh_minesweeper_tile);
    fw64_scene_create_mesh_instance(&game->scene, node, mesh);
    fw64_ui_navigation_init(&game->ui_nav, engine->input, 0);

    set_next_tile(game, 1);
}

void game_update(Game* game){
    fw64_ui_navigation_update(&game->ui_nav, game->engine->time->time_delta);

    if (fw64_ui_navigation_moved_up(&game->ui_nav)) {
        set_next_tile(game, 1);
    } else if (fw64_ui_navigation_moved_down(&game->ui_nav)) {
        set_next_tile(game, -1);
    }
}

void game_fixed_update(Game* game) {
    (void)game;
}

void game_draw(Game* game) {
    fw64_renderpass_begin(game->renderpass);
    fw64_scene_draw_all(&game->scene, game->renderpass);
    fw64_renderpass_end(game->renderpass);
    fw64_renderer_submit_renderpass(game->engine->renderer, game->renderpass);
}
