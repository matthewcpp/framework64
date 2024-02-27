#include "game.h"
#include "assets/assets.h"

#include "framework64/util/mesh_builder.h"

#include "framework64/n64/controller_button.h"

static fw64Mesh* build_rgb_triangle(Game* game) {
    fw64MeshBuilder* mesh_builder = fw64_mesh_builder_create(game->engine->assets, 1, 0, &game->allocator.interface);

    fw64VertexAttributes attributes = FW64_VERTEX_ATTRIBUTE_POSITION | FW64_VERTEX_ATTRIBUTE_COLOR;
    fw64_mesh_builder_allocate_primitive_data(mesh_builder, 0, FW64_PRIMITIVE_MODE_TRIANGLES, attributes, 3, 1);
    fw64_mesh_builder_set_active_primitive(mesh_builder, 0);

    fw64_mesh_builder_set_vertex_position_f(mesh_builder, 0, -1.0f, -1.0f, 0.0f);
    fw64_mesh_builder_set_vertex_color_rgba8(mesh_builder, 0, 0, 0, 255, 255);

    fw64_mesh_builder_set_vertex_position_f(mesh_builder, 1, 1.0f, -1.0f, 0.0f);
    fw64_mesh_builder_set_vertex_color_rgba8(mesh_builder, 1, 0, 255, 0, 255);

    fw64_mesh_builder_set_vertex_position_f(mesh_builder, 2, 0.0f, 1.0f, 0.0f);
    fw64_mesh_builder_set_vertex_color_rgba8(mesh_builder, 2, 255, 0, 0, 255);

    fw64_mesh_builder_set_triangle_vertex_indices(mesh_builder, 0, 0, 1, 2);

    fw64_material_set_shading_mode(fw64_mesh_builder_get_material(mesh_builder, 0), FW64_SHADING_MODE_VERTEX_COLOR);

    Box bounding = {{-1.0f, -1.0f, 0.0f}, {1.0f, 1.0f, 0.0f}};
    fw64_mesh_builder_set_bounding(mesh_builder, &bounding);

    fw64Mesh* mesh = fw64_mesh_builder_commit(mesh_builder);
    fw64_mesh_builder_delete(mesh_builder);

    return mesh;
}

static fw64Mesh* build_textured_quad(Game* game) {
    fw64MeshBuilder* mesh_builder = fw64_mesh_builder_create(game->engine->assets, 1, 1, &game->allocator.interface);

    fw64Texture* texture = fw64_mesh_builder_load_image(mesh_builder, FW64_ASSET_image_fw64_image);
    fw64Material* material = fw64_mesh_builder_get_material(mesh_builder, 0);

    fw64_material_set_shading_mode(material, FW64_SHADING_MODE_UNLIT_TEXTURED);
    fw64_material_set_texture(material, texture, 0);

    fw64VertexAttributes attributes = FW64_VERTEX_ATTRIBUTE_POSITION | FW64_VERTEX_ATTRIBUTE_TEXCOORD;
    fw64_mesh_builder_allocate_primitive_quad_data(mesh_builder, 0, attributes, 16);
    fw64_mesh_builder_set_active_primitive(mesh_builder, 0);

    size_t index = 0;

    const float tc_step = 0.25f;
    const float pos_step = 1.0f;

    for (size_t y = 0; y < 4; y++) {
        float y_pos = 2.0f - (float)y;
        float y_tc = 0.25f * (float)y;

        for (size_t x = 0; x < 4; x++) {
            float x_pos = -2.0f + (float)x;
            float x_tc = 0.25f * (float)x;

            fw64_mesh_builder_set_vertex_position_f(mesh_builder, index, x_pos, y_pos - pos_step, 0.0f);
            fw64_mesh_builder_set_vertex_texcoords_f(mesh_builder, index++, x_tc, y_tc + tc_step);

            fw64_mesh_builder_set_vertex_position_f(mesh_builder, index, x_pos + pos_step, y_pos - pos_step, 0.0f);
            fw64_mesh_builder_set_vertex_texcoords_f(mesh_builder, index++, x_tc + tc_step, y_tc + tc_step);

            fw64_mesh_builder_set_vertex_position_f(mesh_builder, index, x_pos + pos_step, y_pos, 0.0f);
            fw64_mesh_builder_set_vertex_texcoords_f(mesh_builder, index++, x_tc + tc_step, y_tc);

            fw64_mesh_builder_set_vertex_position_f(mesh_builder, index, x_pos, y_pos, 0.0f);
            fw64_mesh_builder_set_vertex_texcoords_f(mesh_builder, index++, x_tc, y_tc);
        }
    }

    Box bounding = {{-2.0f, -2.0f, 0.0f}, {2.0f, 2.0f, 0.0f}};
    fw64_mesh_builder_set_bounding(mesh_builder, &bounding);  

    fw64Mesh* mesh = fw64_mesh_builder_commit(mesh_builder);
    fw64_mesh_builder_delete(mesh_builder);

    return mesh;
}

static fw64Mesh* build_wire_pyramid(Game* game) {
    fw64MeshBuilder* mesh_builder = fw64_mesh_builder_create(game->engine->assets, 1, 0, &game->allocator.interface);

    fw64VertexAttributes attributes = FW64_VERTEX_ATTRIBUTE_POSITION | FW64_VERTEX_ATTRIBUTE_COLOR;
    fw64_mesh_builder_allocate_primitive_data(mesh_builder, 0, FW64_PRIMITIVE_MODE_LINES, attributes, 5, 8);
    fw64_mesh_builder_set_active_primitive(mesh_builder, 0);
    fw64_material_set_shading_mode(fw64_mesh_builder_get_material(mesh_builder, 0), FW64_SHADING_MODE_LINE);

    fw64_mesh_builder_set_vertex_position_f(mesh_builder, 0, 0.0f, 2.0f, 0.0f);
    fw64_mesh_builder_set_vertex_color_rgba8(mesh_builder,0, 255, 255, 0, 255);

    fw64_mesh_builder_set_vertex_position_f(mesh_builder, 1, -1.0f, -1.0f, 1.0f);
    fw64_mesh_builder_set_vertex_color_rgba8(mesh_builder, 1, 255, 255, 0, 255);

    fw64_mesh_builder_set_vertex_position_f(mesh_builder, 2, 1.0f, -1.0f, 1.0f);
    fw64_mesh_builder_set_vertex_color_rgba8(mesh_builder, 2, 255, 255, 0, 255);

    fw64_mesh_builder_set_vertex_position_f(mesh_builder, 3, 1.0f, -1.0f, -1.0f);
    fw64_mesh_builder_set_vertex_color_rgba8(mesh_builder, 3, 255, 255, 0, 255);

    fw64_mesh_builder_set_vertex_position_f(mesh_builder, 4, -1.0f, -1.0f, -1.0f);
    fw64_mesh_builder_set_vertex_color_rgba8(mesh_builder, 4, 255, 255, 0, 255);

    fw64_mesh_builder_set_line_vertex_indices(mesh_builder, 0, 1, 2);
    fw64_mesh_builder_set_line_vertex_indices(mesh_builder, 1, 2, 3);
    fw64_mesh_builder_set_line_vertex_indices(mesh_builder, 2, 3, 4);
    fw64_mesh_builder_set_line_vertex_indices(mesh_builder, 3, 4, 1);

    fw64_mesh_builder_set_line_vertex_indices(mesh_builder, 4, 0, 1);
    fw64_mesh_builder_set_line_vertex_indices(mesh_builder, 5, 0, 2);
    fw64_mesh_builder_set_line_vertex_indices(mesh_builder, 6, 0, 3);
    fw64_mesh_builder_set_line_vertex_indices(mesh_builder, 7, 0, 4);

    Box bounding = {{-1.0f, 0.0f, -1.0f}, {1.0f, 3.0f, 1.0f}};
    fw64_mesh_builder_set_bounding(mesh_builder, &bounding);

    fw64Mesh* mesh = fw64_mesh_builder_commit(mesh_builder);
    fw64_mesh_builder_delete(mesh_builder);

    return mesh;
}

void next_game_state(Game* game, int direction) {
    if (game->game_state != GAME_STATE_NONE) {
        fw64_mesh_delete(game->node.mesh, game->engine->assets, &game->allocator.interface);
    }

    fw64_bump_allocator_reset(&game->allocator);

    game->game_state += direction;
    if (game->game_state == GAME_STATE_NONE) {
        game->game_state = GAME_STATE_COUNT - 1;
    } else if (game->game_state == GAME_STATE_COUNT) {
        game->game_state = GAME_STATE_NONE + 1;
    }

    fw64Mesh* mesh = NULL;

    switch (game->game_state) {
        case GAME_STATE_TRIANGLES:
            mesh = build_rgb_triangle(game);
            break;
    
        case GAME_STATE_QUADS:
            mesh = build_textured_quad(game);
            break;

        case GAME_STATE_LINES:
            mesh = build_wire_pyramid(game);
            break;

        case GAME_STATE_NONE:
        case GAME_STATE_COUNT:
            break;
    }

    fw64_node_set_mesh(&game->node, mesh);
}

void game_init(Game* game, fw64Engine* engine) {
    game->engine = engine;
    fw64_camera_init(&game->camera, fw64_displays_get_primary(engine->displays));

    fw64_node_init(&game->node);
    fw64_bump_allocator_init(&game->allocator, 1024 * 100);

    game->game_state = GAME_STATE_NONE;
    next_game_state(game, 1);
}

void game_update(Game* game){
    fw64Input* input = game->engine->input;
    if (fw64_input_controller_button_pressed(input, 0, FW64_N64_CONTROLLER_BUTTON_L)) {
        next_game_state(game, -1);
    } else if (fw64_input_controller_button_pressed(input, 0, FW64_N64_CONTROLLER_BUTTON_R)) {
        next_game_state(game, 1);
    }
}

void game_draw(Game* game) {
    fw64PrimitiveMode primitive_mode;
    if (game->game_state == GAME_STATE_LINES) {
        primitive_mode = FW64_PRIMITIVE_MODE_LINES;
    } else {
        primitive_mode = FW64_PRIMITIVE_MODE_TRIANGLES;
    }

    fw64_renderer_begin(game->engine->renderer, primitive_mode, FW64_RENDERER_FLAG_CLEAR);
    fw64_renderer_set_camera(game->engine->renderer, &game->camera);
    fw64_renderer_draw_static_mesh(game->engine->renderer, &game->node.transform, game->node.mesh);
    fw64_renderer_end(game->engine->renderer, FW64_RENDERER_FLAG_SWAP);
}
