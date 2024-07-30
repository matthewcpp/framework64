#include "game.h"
#include "assets/assets.h"

#include "framework64/controller_mapping/n64.h"
#include "framework64/math.h"
#include "framework64/util/renderpass_util.h"

#include <stdio.h>

static void color_editor_init(ColorEditor* color_editor, fw64UiNavigation* ui_nav, fw64ColorRGBA8 initial_color, ColorEditorCallback callback, void* arg);
static void color_editor_update(ColorEditor* color_editor, float time_delta);

static void material_editor_init(MaterialEditor* editor, fw64UiNavigation* ui_nav, fw64Mesh* mesh, IVec2* pos, fw64Font* font);
static void material_editor_update(MaterialEditor* editor, float time_delta);
static void material_editor_draw(MaterialEditor* editor, fw64SpriteBatch* spritebatch);
static void material_editor_activate(MaterialEditor* editor);
static void material_editor_deactivate(MaterialEditor* editor);

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

void color_editor_init(ColorEditor* color_editor, fw64UiNavigation* ui_nav, fw64ColorRGBA8 initial_color, ColorEditorCallback callback, void* arg) {
    color_editor->ui_nav = ui_nav;
    color_editor->callback = callback;
    color_editor->callback_arg = arg;
    color_editor->current_color = initial_color;
    color_editor->component_index = 0;
}

#define EDIT_SPEED 225.0f

void color_editor_update(ColorEditor* color_editor, float time_delta) {
    float direction = 0.0f;

    if (fw64_ui_navigation_moved_up(color_editor->ui_nav)) {
        direction = 1.0f;
    } else if (fw64_ui_navigation_moved_down(color_editor->ui_nav)) {
        direction = -1.0f;
    } else if (fw64_ui_navigation_moved_right(color_editor->ui_nav)) {
        color_editor->component_index += 1;
    } else if (fw64_ui_navigation_moved_left(color_editor->ui_nav)) {
        color_editor->component_index -= 1;
    }

    if (color_editor->component_index < 0) {
        color_editor->component_index = 2;
    } else if (color_editor->component_index > 2) {
        color_editor->component_index = 0;
    }

    uint8_t* values = (uint8_t*)&color_editor->current_color;
    int delta = (int)(direction * EDIT_SPEED * time_delta);
    int new_value = (int)(values[color_editor->component_index]) + delta;

    values[color_editor->component_index] = (uint8_t)fw64_clampi(new_value, 0, 255);

    if (color_editor->callback) {
        color_editor->callback(color_editor->callback_arg);
    }
}

static void material_editor_update_str(MaterialEditor* editor) {
    ColorEditor* color_editor = &editor->color_editor;
    const char* r_label = " ", *g_label = " ", *b_label = " ";

    if (editor->active) {
        switch(editor->color_editor.component_index) {
            case 0:
                r_label = ">";
                break;
            case 1:
                g_label = ">";
                break;
            case 2:
                b_label = ">";
                break;
        }
    }

    sprintf(editor->display_str, "Material: %s%d, %s%d, %s%d", 
        r_label, color_editor->current_color.r, 
        g_label, color_editor->current_color.g, 
        b_label, color_editor->current_color.b);
}

static void material_editor_color_updated(void* arg) {
    MaterialEditor* editor = (MaterialEditor*)arg;

    int primitive_count = fw64_mesh_get_primitive_count(editor->mesh);
    for (int i = 0; i < primitive_count; i++) {
        fw64Material* material = fw64_mesh_get_material_for_primitive(editor->mesh, i);
        fw64_material_set_color(material, editor->color_editor.current_color);
    }

    material_editor_update_str(editor);
}

void material_editor_init(MaterialEditor* editor, fw64UiNavigation* ui_nav, fw64Mesh* mesh, IVec2* pos, fw64Font* font){
    editor->mesh = mesh;
    editor->pos = *pos;
    editor->font = font;
    editor->active = 0;

    fw64Material* material = fw64_mesh_get_material_for_primitive(mesh, 0);
    editor->default_color = fw64_material_get_color(material);
    color_editor_init(&editor->color_editor, ui_nav, editor->default_color, material_editor_color_updated, editor);
    material_editor_update_str(editor);
}

void material_editor_draw(MaterialEditor* editor, fw64SpriteBatch* spritebatch) {
    fw64_spritebatch_draw_string(spritebatch, editor->font, editor->display_str, editor->pos.x, editor->pos.y);
}

void material_editor_update(MaterialEditor* editor, float time_delta) {
    color_editor_update(&editor->color_editor, time_delta);
}

void material_editor_activate(MaterialEditor* editor) {
    editor->active = 1;
    material_editor_update_str(editor);
}

void material_editor_deactivate(MaterialEditor* editor) {
    editor->active = 0;
    material_editor_update_str(editor);
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
