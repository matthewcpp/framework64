#include "game.h"
#include "assets/assets.h"
#include "framework64/util/quad.h"
#include "framework64/util/renderpass_util.h"
#include "framework64/util/text_util.h"
#include "framework64/controller_mapping/n64.h"

#include "assets/assets.h"

static void set_next_wrap_mode(Game* game, int direction);

void game_init(Game* game, fw64Engine* engine) {
    fw64Allocator* allocator = fw64_default_allocator();
    fw64Display* display = fw64_displays_get_primary(engine->displays);

    game->engine = engine;
    game->mode = MODE_DEFAULT;
    game->font = fw64_assets_load_font(engine->assets, FW64_ASSET_font_Consolas12, fw64_default_allocator());
    game->spritebatch = fw64_spritebatch_create(1, allocator);
    game->mode = -1;
    game->mesh_node = NULL;

    fw64SceneInfo info;
    fw64_scene_info_init(&info);
    info.node_count = 2;
    info.mesh_instance_count = 1;
    fw64_scene_init(&game->scene, &info, engine->assets, allocator);

    fw64Node* camera_node = fw64_scene_create_node(&game->scene);
    vec3_set(&camera_node->transform.position, 0.0f, 0.0f, 5.0f);
    fw64_node_update(camera_node);
    fw64Camera camera;
    fw64_camera_init(&camera, camera_node, display);

    game->renderpasses[RENDER_PASS_SCENE] = fw64_renderpass_create(display, allocator);
    fw64_renderpass_set_clear_color(game->renderpasses[RENDER_PASS_SCENE], 39, 58, 93);
    fw64_renderpass_set_camera(game->renderpasses[RENDER_PASS_SCENE], &camera);

    game->renderpasses[RENDER_PASS_UI] = fw64_renderpass_create(display, allocator);
    fw64_renderpass_util_ortho2d(game->renderpasses[RENDER_PASS_UI]);

    fw64_bump_allocator_init(&game->mesh_allocator, 5120);
    fw64_ui_navigation_init(&game->ui_nav, engine->input, 0);
    set_next_wrap_mode(game, 1);
}

void set_next_wrap_mode(Game* game, int direction) {
    game->mode += direction;

    if (game->mode >= MODE_COUNT) {
        game->mode = 0;
    } else if (game->mode < 0) {
        game->mode = MODE_COUNT - 1;
    }

    fw64TextureWrapMode wrap_mode = FW64_TEXTURE_WRAP_CLAMP;
    fw64TexturedQuadParams params;
    fw64_textured_quad_params_init(&params);
    params.image_asset_id = FW64_ASSET_image_pyoro64;
    params.shading_mode = FW64_SHADING_MODE_UNLIT_TRANSPARENT_TEXTURED;

    const char* mode_name = NULL;

    switch (game->mode)
    {
        case MODE_DEFAULT:
            mode_name = "Default";
            params.max_s = 1.0f;
            params.max_t = 1.0f;
            wrap_mode = FW64_TEXTURE_WRAP_CLAMP;
            break;

        case MODE_CLAMP:
            mode_name = "Clamp";
            params.max_s = 2.0f;
            params.max_t = 2.0f;
            wrap_mode = FW64_TEXTURE_WRAP_CLAMP;
            break;

        case MODE_WRAP:
            mode_name = "Repeat";
            params.max_s = 2.0f;
            params.max_t = 2.0f;
            wrap_mode = FW64_TEXTURE_WRAP_REPEAT;
            break;

        case MODE_MIRROR:
            mode_name = "Mirror";
            params.max_s = 2.0f;
            params.max_t = 2.0f;
            wrap_mode = FW64_TEXTURE_WRAP_MIRROR;
            break;
    }

    fw64Allocator* mesh_allocator = &game->mesh_allocator.interface;

    fw64Mesh* mesh;
    if (game->mesh_node) {
        fw64_mesh_delete(game->mesh_node->mesh_instance->mesh, game->engine->assets, mesh_allocator);
        fw64_bump_allocator_reset(&game->mesh_allocator);
        mesh = fw64_textured_quad_create_with_params(game->engine, &params, mesh_allocator);
        fw64_mesh_instance_set_mesh(game->mesh_node->mesh_instance, mesh);
    } else {
        mesh = fw64_textured_quad_create_with_params(game->engine, &params, mesh_allocator);
        game->mesh_node = fw64_scene_create_node(&game->scene);
        fw64_scene_create_mesh_instance(&game->scene, game->mesh_node, mesh);
    }

    fw64Material* material = fw64_material_collection_get_material(fw64_mesh_get_material_collection(mesh), 0);
    fw64Texture* texture = fw64_material_get_texture(material);
    fw64_texture_set_wrap_mode(texture, wrap_mode, wrap_mode);
    const fw64Viewport* viewport = fw64_renderpass_get_viewport(game->renderpasses[RENDER_PASS_UI]);
    IVec2 pos = fw64_text_util_center_string(game->font, mode_name, &viewport->size);
    fw64_spritebatch_begin(game->spritebatch);
    fw64_spritebatch_draw_string(game->spritebatch, game->font, mode_name, pos.x, 10);
    fw64_spritebatch_end(game->spritebatch);
}

void game_update(Game* game){
    fw64_ui_navigation_update(&game->ui_nav, game->engine->time->time_delta);

    if (fw64_ui_navigation_moved_right(&game->ui_nav)) {
        set_next_wrap_mode(game, 1);
    } else if (fw64_ui_navigation_moved_left(&game->ui_nav)) {
        set_next_wrap_mode(game, -1);
    } 
}

void game_draw(Game* game) {
    fw64RenderPass* renderpass = game->renderpasses[RENDER_PASS_SCENE];
    fw64_renderpass_begin(renderpass);
    fw64_scene_draw_all(&game->scene, renderpass);
    fw64_renderpass_end(renderpass);
    fw64_renderer_submit_renderpass(game->engine->renderer, renderpass);

    renderpass = game->renderpasses[RENDER_PASS_UI];
    fw64_renderpass_begin(renderpass);
    fw64_renderpass_draw_sprite_batch(renderpass, game->spritebatch);
    fw64_renderpass_end(renderpass);
    fw64_renderer_submit_renderpass(game->engine->renderer, renderpass);
}
