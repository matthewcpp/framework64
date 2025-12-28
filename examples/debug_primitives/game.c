#include "game.h"
#include "assets/assets.h"
#include "assets/scene_debug_primitives.h"
#include "assets/scene_wire_primitives.h"

#include <framework64/util/renderpass_util.h>
#include <framework64/math.h>

#include "framework64/controller_mapping/n64.h"

static void ui_init(UI* ui, fw64Engine* engine, fw64Scene* scene, fw64DebugPrimitives* debug_primitives, fw64Display* display, fw64Allocator* allocator);
static void ui_update(UI* ui);
static void ui_draw(UI* ui);

#define EDIT_NODES_COUNT 3
static fw64DebugPrimitiveHandle debug_handles[3];

static const fw64NodeIndex edit_nodes[EDIT_NODES_COUNT] = {
    FW64_scene_debug_primitives_node_aabb,
    FW64_scene_debug_primitives_node_sphere,
    FW64_scene_debug_primitives_node_capsule
};

static void node_to_capsule(fw64Node* node, fw64Capsule* capsule);
static void node_to_sphere(fw64Node* node, Vec3* origin, float* radius);
static void node_to_box(fw64Node* node, Box* box);

void game_init(Game* game, fw64Engine* engine) {
    fw64Allocator* allocator = fw64_default_allocator();
    fw64Display* display = fw64_displays_get_primary(engine->displays);

    game->engine = engine;
    game->scene_renderpass = fw64_renderpass_create(display, allocator);
    game->debug_renderpass = fw64_renderpass_create(display, allocator);
    fw64_renderpass_set_primitive_mode(game->debug_renderpass, FW64_PRIMITIVE_MODE_LINES);
    fw64_renderpass_set_depth_testing_enabled(game->debug_renderpass, 0);

    game->scene = fw64_assets_load_scene(engine->assets, FW64_ASSET_scene_debug_primitives, allocator);

    fw64_camera_init(&game->camera, fw64_scene_get_node(game->scene, FW64_scene_debug_primitives_node_camera), display);
    fw64_renderpass_set_camera(game->scene_renderpass, &game->camera);
    fw64_renderpass_set_camera(game->debug_renderpass, &game->camera);
    fw64_headlight_init(&game->headlight, game->scene_renderpass, 0, &game->camera.node->transform);
    fw64_renderpass_set_light_enabled(game->scene_renderpass, 0, 1);

    fw64DebugPrimitivesConfig config = {
        FW64_ASSET_scene_wire_primitives,
        FW64_scene_wire_primitives_node__capsule_stem,
        FW64_scene_wire_primitives_node__capsule_bottom,
        FW64_scene_wire_primitives_node__capsule_top,
        FW64_scene_wire_primitives_node__sphere,
        FW64_scene_wire_primitives_node__box,
        1.0f / 10.0f,
        1,
        1,
        1
    };
    fw64_debug_primitives_init(&game->debug_primitives, engine, &config, allocator);

    Box box;
    node_to_box(fw64_scene_get_node(game->scene, FW64_scene_debug_primitives_node_aabb), &box);
    debug_handles[0] = fw64_debug_primitives_add_box(&game->debug_primitives, &box);

    Vec3 center;
    float radius;
    node_to_sphere(fw64_scene_get_node(game->scene, FW64_scene_debug_primitives_node_sphere), &center, &radius);
    debug_handles[1] = fw64_debug_primitives_add_sphere(&game->debug_primitives, &center, radius);

    fw64Capsule capsule;
    node_to_capsule(fw64_scene_get_node(game->scene, FW64_scene_debug_primitives_node_capsule), &capsule);
    debug_handles[2] = fw64_debug_primitives_add_capsule(&game->debug_primitives, &capsule);

    ui_init(&game->ui, engine, game->scene, &game->debug_primitives, display, allocator);
}

static void node_to_capsule(fw64Node* node, fw64Capsule* capsule) {
    // determine base capsule sizes based on the inital state of the mesh
    static float base_capsule_radius, base_capsule_stem_extent = -1.0f;
    if (base_capsule_stem_extent < 0.0f) {
        Vec3 extents;
        box_extents(&node->mesh_instance->render_bounds, &extents);
        base_capsule_stem_extent = extents.y / node->transform.scale.y;
        base_capsule_radius = fw64_maxf(extents.x / node->transform.scale.x, extents.z / node->transform.scale.z);
    }

    Vec3 base, tip, axis = vec3_up(); // the the default capsure state is vertical in Y
    quat_transform_vec3(&node->transform.rotation, &axis, &axis);
    vec3_normalize(&axis);

    float radius = base_capsule_radius * fw64_maxf(node->transform.scale.x, node->transform.scale.z);
    float stem_extent = base_capsule_stem_extent * node->transform.scale.y;
    
    vec3_add_and_scale(&node->transform.position, &axis, -stem_extent, &base);
    vec3_add_and_scale(&node->transform.position, &axis, stem_extent, &tip);
    fw64_capsule_init(capsule, &base, &tip, radius);
}

static void node_to_sphere(fw64Node* node, Vec3* origin, float* radius) {
    Vec3 extents;
    box_center(&node->mesh_instance->render_bounds, origin);
    box_extents(&node->mesh_instance->render_bounds, &extents);
    *radius = extents.x;
}

static void node_to_box(fw64Node* node, Box* box){
    *box = node->mesh_instance->render_bounds;
}

void game_update(Game* game){
    fw64_headlight_update(&game->headlight);
    ui_update(&game->ui);
}

void game_fixed_update(Game* game){
    (void)game;
}

void game_draw(Game* game) {
    fw64_renderpass_begin(game->scene_renderpass);
    fw64_scene_draw_all(game->scene, game->scene_renderpass, FW64_LAYER_MASK_ALL_LAYERS);
    fw64_renderpass_end(game->scene_renderpass);

    fw64_renderpass_begin(game->debug_renderpass);
    fw64_debug_primitives_draw(&game->debug_primitives, game->debug_renderpass);
    fw64_renderpass_end(game->debug_renderpass);

    fw64_renderer_submit_renderpass(game->engine->renderer, game->scene_renderpass);
    ui_draw(&game->ui);
    fw64_renderer_submit_renderpass(game->engine->renderer, game->debug_renderpass);
}

static void ui_edit_next_node(UI* ui, int direction);

void ui_init(UI* ui, fw64Engine* engine, fw64Scene* scene, fw64DebugPrimitives* debug_primitives, fw64Display* display, fw64Allocator* allocator) {
    ui->engine = engine;
    ui->scene = scene;
    ui->debug_primitives = debug_primitives;
    ui->renderpass = fw64_renderpass_create(display, allocator);
    fw64_renderpass_util_ortho2d(ui->renderpass);

    ui->font = fw64_assets_load_font(engine->assets, FW64_ASSET_font_Consolas12, allocator);
    fw64_transform_editor_init(&ui->editor, engine, ui->font, NULL, allocator);
    ivec2_set(&ui->editor.ui_pos, 10, 10);
    fw64_ui_navigation_disable_lr(&ui->editor.ui_navigation);

    ui->node_edit_index = EDIT_NODES_COUNT;
    ui_edit_next_node(ui, 1);
}

void ui_edit_next_node(UI* ui, int direction) {
    ui->node_edit_index += direction;
    if (ui->node_edit_index >= EDIT_NODES_COUNT) {
        ui->node_edit_index = 0;
    } else if (ui->node_edit_index < 0) {
        ui->node_edit_index = EDIT_NODES_COUNT - 1;
    }
    
    fw64Node* node = fw64_scene_get_node(ui->scene, edit_nodes[ui->node_edit_index]);
    fw64_transform_editor_set_target(&ui->editor, node);
}

void ui_update(UI* ui) {
    if (fw64_input_controller_button_released(ui->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_R)) {
        ui_edit_next_node(ui, 1);
    } else if (fw64_input_controller_button_released(ui->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_L)){
        ui_edit_next_node(ui, -1);
    }
    fw64_transform_editor_update(&ui->editor);
    if (!ui->editor.did_edit) {
        return;
    }

    switch (edit_nodes[ui->node_edit_index]) {
        case FW64_scene_debug_primitives_node_aabb: {
            Box box;
            node_to_box(fw64_scene_get_node(ui->scene, FW64_scene_debug_primitives_node_aabb), &box);
            fw64_debug_primitives_update_box(ui->debug_primitives, debug_handles[ui->node_edit_index], &box);
            break;
        }

        case FW64_scene_debug_primitives_node_sphere: {
            Vec3 center;
            float radius;
            node_to_sphere(fw64_scene_get_node(ui->scene, FW64_scene_debug_primitives_node_sphere), &center, &radius);
            fw64_debug_primitives_update_sphere(ui->debug_primitives, debug_handles[ui->node_edit_index], &center, radius);
            break;
        }

        case FW64_scene_debug_primitives_node_capsule: {
            fw64Capsule capsule;
            node_to_capsule(fw64_scene_get_node(ui->scene, FW64_scene_debug_primitives_node_capsule), &capsule);
            fw64_debug_primitives_update_capsule(ui->debug_primitives, debug_handles[ui->node_edit_index], &capsule);
            break;
        }
    }
}

void ui_draw(UI* ui) {
    fw64_renderpass_begin(ui->renderpass);
    fw64_renderpass_draw_sprite_batch(ui->renderpass, ui->editor.spritebatch);
    fw64_renderpass_end(ui->renderpass);

    fw64_renderer_submit_renderpass(ui->engine->renderer, ui->renderpass);
}
