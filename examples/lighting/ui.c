#include "ui.h"

#include "assets/assets.h"

void ui_init(Ui* ui, fw64Engine* engine, fw64Scene* scene, fw64RenderPass* scene_renderpass, fw64Headlights* headlights, fw64Transform* camera_transform) {
    fw64Allocator* allocator = fw64_default_allocator();

    ui->engine = engine;
    ui->scene = scene;
    ui->setting_index = SETTING_LIGHT1;
    ui->spritebatch = fw64_spritebatch_create(1, fw64_default_allocator());
    ui->setting_index_is_active = 0;
    ui->is_active = 0;

    fw64Font* font = fw64_assets_load_font(engine->assets, FW64_ASSET_font_Consolas12, allocator);

    fw64_ui_navigation_init(&ui->ui_nav, engine->input, 0);

    IVec2 widget_pos = {10, 10};

    material_editor_init(&ui->material_editor, &ui->ui_nav, fw64_scene_get_node(ui->scene, 0), &widget_pos, font);
    widget_pos.y += fw64_font_line_height(font);

    light_editor_init(&ui->light_editors[0], &ui->ui_nav, font, &widget_pos, scene_renderpass, 0, LIGHT_MODE_HEADLIGHT, headlights, camera_transform);
    widget_pos.y += fw64_font_line_height(font);

    light_editor_init(&ui->light_editors[1], &ui->ui_nav, font, &widget_pos, scene_renderpass, 1, LIGHT_MODE_OFF, headlights, camera_transform);
    widget_pos.y += fw64_font_line_height(font);

    light_editor_init(&ui->light_editors[2], &ui->ui_nav, font, &widget_pos, scene_renderpass, 2, LIGHT_MODE_OFF, headlights, camera_transform);
    widget_pos.y += fw64_font_line_height(font);

    ambient_editor_init(&ui->ambient_editor, &ui->ui_nav, scene_renderpass, &widget_pos, font);
}

static void ui_activate_current_control(Ui* ui) {
    switch (ui->setting_index) {
        case SETTING_MATERIAL_COLOR:
            material_editor_activate(&ui->material_editor);
            break;
        case SETTING_LIGHT1:
            light_editor_activate(&ui->light_editors[0]);
            break;
        case SETTING_LIGHT2:
            light_editor_activate(&ui->light_editors[1]);
            break;
        case SETTING_LIGHT3:
            light_editor_activate(&ui->light_editors[2]);
            break;
        case SETTING_AMBIENT:
            ambient_editor_activate(&ui->ambient_editor);
            break;
        case SETTING_INVALID:
            break;
    }
}

static void ui_deactivate_current_control(Ui* ui) {
    switch (ui->setting_index) {
        case SETTING_MATERIAL_COLOR:
            material_editor_deactivate(&ui->material_editor);
            break;
        case SETTING_LIGHT1:
            light_editor_deactivate(&ui->light_editors[0]);
            break;
        case SETTING_LIGHT2:
            light_editor_deactivate(&ui->light_editors[1]);
            break;
        case SETTING_LIGHT3:
            light_editor_deactivate(&ui->light_editors[2]);
            break;
        case SETTING_AMBIENT:
            ambient_editor_deactivate(&ui->ambient_editor);
            break;
        case SETTING_INVALID:
            break;
    }
}

void ui_update(Ui* ui) {
    fw64_ui_navigation_update(&ui->ui_nav, ui->engine->time->time_delta);

    if (ui->setting_index_is_active) {
        switch (ui->setting_index) {
            case SETTING_MATERIAL_COLOR:
                material_editor_update(&ui->material_editor, ui->engine->time->time_delta);
                break;
            case SETTING_LIGHT1:
                light_editor_update(&ui->light_editors[0],  ui->engine->time->time_delta);
                break;
            case SETTING_LIGHT2:
                light_editor_update(&ui->light_editors[1],  ui->engine->time->time_delta);
                break;
            case SETTING_LIGHT3:
                light_editor_update(&ui->light_editors[2],  ui->engine->time->time_delta);
                break;
            case SETTING_AMBIENT:
                ambient_editor_update(&ui->ambient_editor, ui->engine->time->time_delta);
                break;
            case SETTING_INVALID:
                break;
        }

        if (fw64_ui_navigation_back(&ui->ui_nav) || fw64_ui_navigation_accepted(&ui->ui_nav)) {
                ui->setting_index_is_active = 0;
                ui_deactivate_current_control(ui);
        }
    } else {
        if (fw64_ui_navigation_moved_down(&ui->ui_nav)) {
            ui->setting_index += 1;
        } else if (fw64_ui_navigation_moved_up(&ui->ui_nav)) {
            ui->setting_index -= 1;
        } 

        if (ui->setting_index < 0) {
            ui->setting_index = SETTING_INVALID - 1;
        } else if (ui->setting_index >= SETTING_INVALID) {
            ui->setting_index = 0;
        }
        
        if (fw64_ui_navigation_accepted(&ui->ui_nav)) {
            ui->setting_index_is_active = 1;
            ui_activate_current_control(ui);
        } else if (fw64_ui_navigation_back(&ui->ui_nav)) {
            ui->is_active = 0;
        }
    }
}

static void ui_set_color(Ui* ui, SettingIndex index) {
    if (!ui->is_active || ui->setting_index != index) {
        fw64_spritebatch_set_color(ui->spritebatch, 150, 150, 150, 255);
    }
    else{
        if (ui->setting_index_is_active) {
            fw64_spritebatch_set_color(ui->spritebatch, 255, 255, 0, 255);
        } else {
            fw64_spritebatch_set_color(ui->spritebatch, 255, 255, 255, 255);
        }
    }
}

void ui_draw(Ui* ui, fw64RenderPass* renderpass) {
    fw64_spritebatch_begin(ui->spritebatch);
    
    ui_set_color(ui, SETTING_MATERIAL_COLOR);
    material_editor_draw(&ui->material_editor, ui->spritebatch);
    
    ui_set_color(ui, SETTING_LIGHT1);
    light_editor_draw(&ui->light_editors[0], ui->spritebatch);
    
    ui_set_color(ui, SETTING_LIGHT2);
    light_editor_draw(&ui->light_editors[1], ui->spritebatch);

    ui_set_color(ui, SETTING_LIGHT3);
    light_editor_draw(&ui->light_editors[2], ui->spritebatch);

    ui_set_color(ui, SETTING_AMBIENT);
    ambient_editor_draw(&ui->ambient_editor, ui->spritebatch);

    fw64_spritebatch_end(ui->spritebatch);

    fw64_renderpass_draw_sprite_batch(renderpass, ui->spritebatch);
}

void ui_activate(Ui* ui) {
    ui->is_active = 1;
}
