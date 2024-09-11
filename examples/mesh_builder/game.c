#include "game.h"
#include "assets/assets.h"

#include "framework64/util/mesh_builder.h"

#include "framework64/controller_mapping/n64.h"

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

    fw64_material_set_shading_mode(fw64_mesh_builder_get_material(mesh_builder, 0), FW64_SHADING_MODE_UNLIT);

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

    fw64VertexAttributes attributes = FW64_VERTEX_ATTRIBUTE_POSITION | FW64_VERTEX_ATTRIBUTE_TEXCOORD | FW64_VERTEX_ATTRIBUTE_COLOR;
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
            fw64_mesh_builder_set_vertex_color_rgba8(mesh_builder, index, 255, 255, 255, 255);
            fw64_mesh_builder_set_vertex_texcoords_f(mesh_builder, index++, x_tc, y_tc + tc_step);

            fw64_mesh_builder_set_vertex_position_f(mesh_builder, index, x_pos + pos_step, y_pos - pos_step, 0.0f);
            fw64_mesh_builder_set_vertex_color_rgba8(mesh_builder, index, 255, 255, 255, 255);
            fw64_mesh_builder_set_vertex_texcoords_f(mesh_builder, index++, x_tc + tc_step, y_tc + tc_step);

            fw64_mesh_builder_set_vertex_position_f(mesh_builder, index, x_pos + pos_step, y_pos, 0.0f);
            fw64_mesh_builder_set_vertex_color_rgba8(mesh_builder, index, 255, 255, 255, 255);
            fw64_mesh_builder_set_vertex_texcoords_f(mesh_builder, index++, x_tc + tc_step, y_tc);

            fw64_mesh_builder_set_vertex_position_f(mesh_builder, index, x_pos, y_pos, 0.0f);
            fw64_mesh_builder_set_vertex_color_rgba8(mesh_builder, index, 255, 255, 255, 255);
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
        fw64_scene_uninit(&game->scene);
        fw64_mesh_delete(game->mesh, game->engine->assets, &game->allocator.interface);
        game->mesh = NULL;
    }

    fw64_bump_allocator_reset(&game->allocator);

    game->game_state += direction;
    if (game->game_state == GAME_STATE_NONE) {
        game->game_state = GAME_STATE_COUNT - 1;
    } else if (game->game_state == GAME_STATE_COUNT) {
        game->game_state = GAME_STATE_NONE + 1;
    }

    fw64SceneInfo info;
    fw64_scene_info_init(&info);
    info.node_count = 1;
    info.mesh_instance_count = 1;
    fw64_scene_init(&game->scene, &info, game->engine->assets, &game->allocator.interface);

    fw64Node* node = fw64_scene_create_node(&game->scene);

    switch (game->game_state) {
        case GAME_STATE_TRIANGLES:
            game->mesh = build_rgb_triangle(game);
            break;
    
        case GAME_STATE_QUADS:
            game->mesh = build_textured_quad(game);
            break;

        case GAME_STATE_LINES:
            game->mesh = build_wire_pyramid(game);
            break;

        case GAME_STATE_NONE:
        case GAME_STATE_COUNT:
            break;
    }

    fw64_scene_create_mesh_instance(&game->scene, node, game->mesh);
}

void game_init(Game* game, fw64Engine* engine) {
    fw64Display* display = fw64_displays_get_primary(engine->displays);
    game->engine = engine;
    fw64Camera camera;
    fw64_camera_init(&camera, display);

    game->renderpass = fw64_renderpass_create(display, fw64_default_allocator());
    fw64_renderpass_set_camera(game->renderpass, &camera);

    fw64_bump_allocator_init(&game->allocator, 1024 * 100);
    fw64_ui_navigation_init(&game->ui_nav, engine->input, 0);

    fw64SceneInfo scene_info;
    fw64_scene_info_init(&scene_info);
    fw64_scene_init(&game->scene, &scene_info, engine->assets, &game->allocator.interface);

    game->game_state = GAME_STATE_NONE;
    next_game_state(game, 1);
}

void game_update(Game* game){
    fw64_ui_navigation_update(&game->ui_nav, game->engine->time->time_delta);

    if (fw64_ui_navigation_moved_left(&game->ui_nav)) {
        next_game_state(game, -1);
    } else if (fw64_ui_navigation_moved_right(&game->ui_nav)) {
        next_game_state(game, 1);
    }
}

void game_draw(Game* game) {
    fw64PrimitiveMode primitive_mode = fw64_mesh_primitive_get_mode(game->mesh, 0);

    fw64_renderer_begin(game->engine->renderer, primitive_mode, FW64_CLEAR_FLAG_ALL);
    fw64_renderpass_begin(game->renderpass);
    fw64_scene_draw_all(&game->scene, game->renderpass);
    fw64_renderpass_end(game->renderpass);
    fw64_renderer_submit_renderpass(game->engine->renderer, game->renderpass);
    fw64_renderer_end(game->engine->renderer, FW64_RENDERER_FLAG_SWAP);
}
