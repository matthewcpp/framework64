#include "game.h"
#include "assets/assets.h"

#include "framework64/controller_mapping/n64.h"
#include "framework64/util/renderpass_util.h"

static void ui_init(Ui* ui, fw64Engine* engine, fw64Scene* scene);
static void ui_update(Ui* ui);
static void ui_draw(Ui* ui, fw64RenderPass* renderpass);
static void ui_activate(Ui* ui);
static void ui_deactivate(Ui* ui);

void game_init(Game* game, fw64Engine* engine) {
    fw64Allocator* allocator = fw64_default_allocator();
    fw64Display* display = fw64_displays_get_primary(engine->displays);

    game->engine = engine;

    game->renderpasses[RENDER_PASS_SCENE] = fw64_renderpass_create(display, allocator);
    game->renderpasses[RENDER_PASS_UI] = fw64_renderpass_create(display, allocator);
    fw64_renderpass_util_ortho2d(game->renderpasses[RENDER_PASS_UI]);

    game->mode = GAME_MODE_SCENE;

    fw64SceneInfo info;
    fw64_scene_info_init(&info);
    info.mesh_count = 1;
    info.node_count = 1;
    info.mesh_instance_count = 1;
    fw64_scene_init(&game->scene, &info, engine->assets, allocator);

    fw64Mesh* mesh = fw64_scene_load_mesh_asset(&game->scene, FW64_ASSET_mesh_suzanne);
    fw64Node* node = fw64_scene_create_node(&game->scene);
    fw64_scene_create_mesh_instance(&game->scene, node, mesh);

    fw64_arcball_init(&game->arcball, engine->input, display);
    Box mesh_bounding = fw64_mesh_get_bounding_box(mesh);
    fw64_arcball_set_initial(&game->arcball, &mesh_bounding);
    fw64_headlight_init(&game->headlight, game->renderpasses[RENDER_PASS_SCENE], 0, &game->arcball.camera);

    ui_init(&game->ui, engine, &game->scene);
}

void game_update(Game* game){
    if (fw64_input_controller_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_START)) {
        if (game->mode == GAME_MODE_SCENE) {
            game->mode = GAME_MODE_MENU;
            ui_activate(&game->ui);
        } else {
            game->mode = GAME_MODE_SCENE;
            ui_deactivate(&game->ui);
        }

        return;
    }

    if (game->mode == GAME_MODE_SCENE) {
        fw64_arcball_update(&game->arcball, game->engine->time->time_delta);
        fw64_headlight_update(&game->headlight);
    } else {
        ui_update(&game->ui);
    }
}

void game_draw(Game* game) {
    fw64_renderer_begin(game->engine->renderer, FW64_PRIMITIVE_MODE_TRIANGLES, FW64_CLEAR_FLAG_ALL);
    fw64RenderPass* renderpass = game->renderpasses[RENDER_PASS_SCENE];
    fw64_renderpass_set_camera(renderpass, &game->arcball.camera);
    fw64_renderpass_begin(renderpass);
    fw64_scene_draw_all(&game->scene, renderpass);
    fw64_renderpass_end(renderpass);
    fw64_renderer_submit_renderpass(game->engine->renderer, renderpass);

    renderpass = game->renderpasses[RENDER_PASS_UI];
    fw64_renderpass_begin(renderpass);
    ui_draw(&game->ui, renderpass);
    fw64_renderpass_end(renderpass);
    fw64_renderer_submit_renderpass(game->engine->renderer, renderpass);

    fw64_renderer_end(game->engine->renderer, FW64_RENDERER_FLAG_SWAP);
}

static void ui_init(Ui* ui, fw64Engine* engine, fw64Scene* scene) {
    fw64Allocator* allocator = fw64_default_allocator();

    ui->engine = engine;
    ui->scene = scene;
    ui->setting_index = SETTING_MATERIAL_COLOR;
    ui->spritebatch = fw64_spritebatch_create(1, fw64_default_allocator());

    fw64Font* font = fw64_assets_load_font(engine->assets, FW64_ASSET_font_Consolas12, allocator);

    fw64_ui_navigation_init(&ui->ui_nav, engine->input, 0);

    IVec2 widget_pos = {10, 10};
    material_editor_init(&ui->material_editor, &ui->ui_nav, fw64_scene_get_mesh(ui->scene, 0), &widget_pos, font);
}

static void ui_update(Ui* ui) {
    fw64_ui_navigation_update(&ui->ui_nav, ui->engine->time->time_delta);

    switch (ui->setting_index) {
        case SETTING_MATERIAL_COLOR:
            material_editor_update(&ui->material_editor, ui->engine->time->time_delta);
            break;
    }
}

void ui_draw(Ui* ui, fw64RenderPass* renderpass) {
    fw64_spritebatch_begin(ui->spritebatch);
    material_editor_draw(&ui->material_editor, ui->spritebatch);
    fw64_spritebatch_end(ui->spritebatch);

    fw64_renderpass_draw_sprite_batch(renderpass, ui->spritebatch);
}

void ui_activate(Ui* ui) {
    material_editor_activate(&ui->material_editor);
}

void ui_deactivate(Ui* ui) {
    material_editor_deactivate(&ui->material_editor);
}
