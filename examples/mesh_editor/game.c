#include "game.h"

#include "framework64/util/mesh_editor.h"

#include "assets/assets.h"

#include "framework64/controller_mapping/n64.h"

#define TILE_TEX_COORD_COUNT 1

Vec2 tile_tex_coords[TILE_TEX_COORD_COUNT] = {
    {0.25, 0}
};

static void set_tile_tex_coords(Game* game, size_t index) {
    Vec2* pos = &tile_tex_coords[0] + index;
    fw64MeshEditor mesh_editor;
    fw64_mesh_editor_init(&mesh_editor, game->mesh);
    fw64_mesh_editor_set_active_primitive(&mesh_editor, 0);

    fw64_mesh_editor_set_vertex_texcoords_f(&mesh_editor, 0, pos->x, pos->y + 0.25f);
    fw64_mesh_editor_set_vertex_texcoords_f(&mesh_editor, 1, pos->x + 0.25f, pos->y + 0.25f);
    fw64_mesh_editor_set_vertex_texcoords_f(&mesh_editor, 2, pos->x + 0.25f, pos->y);
    fw64_mesh_editor_set_vertex_texcoords_f(&mesh_editor, 3, pos->x, pos->y);

    fw64_mesh_editor_commit(&mesh_editor);
}

void game_init(Game* game, fw64Engine* engine) {
    game->engine = engine;
    fw64_camera_init(&game->camera, fw64_displays_get_primary(engine->displays));
    fw64_bump_allocator_init(&game->bump_allocator, 64 * 1024);

    game->mesh = fw64_assets_load_mesh(engine->assets, FW64_ASSET_mesh_minesweeper_tile, &game->bump_allocator.interface);
    fw64_node_init(&game->node);
    fw64_node_set_mesh(&game->node, game->mesh);

    set_tile_tex_coords(game, 0);
}

void game_update(Game* game){
    (void)game;
}

void game_draw(Game* game) {
    fw64_renderer_begin(game->engine->renderer, FW64_PRIMITIVE_MODE_TRIANGLES, FW64_RENDERER_FLAG_CLEAR);
    fw64_renderer_set_camera(game->engine->renderer, &game->camera);
    fw64_renderer_draw_static_mesh(game->engine->renderer, &game->node.transform, game->node.mesh);
    fw64_renderer_end(game->engine->renderer, FW64_RENDERER_FLAG_SWAP);
}
