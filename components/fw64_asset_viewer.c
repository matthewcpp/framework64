#include "fw64_asset_viewer.h"

#include "framework64/util/renderpass_util.h"
#include "framework64/util/text_util.h"

#include <string.h>

static void unknown_state_init(fw64AssetViewerUnknownState* state, const char* message, const char* path);
static void unknown_state_uninit(fw64AssetViewerUnknownState* state);
static void unknown_state_update(fw64AssetViewerUnknownState* state);
static void unknown_state_draw(fw64AssetViewerUnknownState* state);

static int mesh_state_init(fw64AssetViewerMeshState* state, fw64DataSource* data_reader);
static void mesh_state_uninit(fw64AssetViewerMeshState* state);
static void mesh_state_update(fw64AssetViewerMeshState* state);
static void mesh_state_draw(fw64AssetViewerMeshState* state);

static void uninit_current_state(fw64AssetViewer* file_viewer);

void fw64_asset_viewer_init(fw64AssetViewer* file_viewer, fw64Engine* engine, fw64RenderPass* renderpass, fw64Font* font, uint32_t data_size) {
    file_viewer->state.base.engine = engine;
    file_viewer->state.base.media = fw64_modules_get_static(engine->modules, FW64_MEDIA_MODULE_ID);
    file_viewer->state.base.font = font;
    file_viewer->state.base.renderpass = renderpass;
    
    fw64_bump_allocator_init(&file_viewer->state.base.allocator, data_size);

    file_viewer->state.type = FW64_ASSET_TYPE_UNKNOWN;
    unknown_state_init(&file_viewer->state.unknown, "No Asset Loaded", NULL);
}

void fw64_asset_viewer_uninit(fw64AssetViewer* file_viewer) {
    uninit_current_state(file_viewer);
    fw64_bump_allocator_uninit(&file_viewer->state.base.allocator);
}

void uninit_current_state(fw64AssetViewer* file_viewer) {
    switch (file_viewer->state.type) {
        case FW64_ASSET_TYPE_UNKNOWN:
            unknown_state_uninit(&file_viewer->state.unknown);
            break;

        case FW64_ASSET_TYPE_MESH:
            mesh_state_uninit(&file_viewer->state.mesh);
            break;
    }

    fw64_bump_allocator_reset(&file_viewer->state.base.allocator);
}

void fw64_asset_viewer_set_empty_state(fw64AssetViewer* file_viewer, const char* message, const char* path) {
    uninit_current_state(file_viewer);
    file_viewer->state.type = FW64_ASSET_TYPE_UNKNOWN;
    unknown_state_init(&file_viewer->state.unknown, message, path);
}

void fw64_asset_viewer_update(fw64AssetViewer* file_viewer) {
    switch (file_viewer->state.type) {
        case FW64_ASSET_TYPE_UNKNOWN:
            unknown_state_update(&file_viewer->state.unknown);
            break;

        case FW64_ASSET_TYPE_MESH:
            mesh_state_update(&file_viewer->state.mesh);
            break;
    }
}

void fw64_asset_viewer_draw(fw64AssetViewer* file_viewer) {
    fw64_renderpass_begin(file_viewer->state.base.renderpass);

    switch (file_viewer->state.type) {
        case FW64_ASSET_TYPE_UNKNOWN:
            unknown_state_draw(&file_viewer->state.unknown);
            break;

        case FW64_ASSET_TYPE_MESH:
            mesh_state_draw(&file_viewer->state.mesh);
            break;
    }

    fw64_renderpass_end(file_viewer->state.base.renderpass);
}

void fw64_asset_viewer_load_from_media(fw64AssetViewer* file_viewer, const char* path) {
    fw64AssetType type = fw64_asset_viewer_determine_type(path);

    if (type == FW64_ASSET_TYPE_UNKNOWN) {
        fw64_asset_viewer_set_empty_state(file_viewer, "Unknown asset type:", path);
        return;
    }

    fw64DataSource* data_reader = fw64_media_open_data_source(file_viewer->state.base.media, path);

    if (!data_reader) {
        fw64_asset_viewer_set_empty_state(file_viewer, "File open failed:", path);
        return;
    }
    fw64_asset_viewer_load_data(file_viewer, data_reader, type);
    fw64_media_close_data_source(file_viewer->state.base.media, data_reader);
}

void fw64_asset_viewer_load_data(fw64AssetViewer* file_viewer, fw64DataSource* data_reader, fw64AssetType type) {
    uninit_current_state(file_viewer);

    file_viewer->state.type = type;

    int result = 1;

    switch (file_viewer->state.type) {
        case FW64_ASSET_TYPE_MESH:
            result = mesh_state_init(&file_viewer->state.mesh, data_reader);
            break;

        case FW64_ASSET_TYPE_UNKNOWN:
            unknown_state_init(&file_viewer->state.unknown, "Unknown asset type", NULL);
            break;
    }

    if (!result) {
        fw64_asset_viewer_set_empty_state(file_viewer, "Asset loading failed", NULL);
    }
}

void unknown_state_init(fw64AssetViewerUnknownState* state, const char* message, const char* path) {
    state->spritebatch = fw64_spritebatch_create(1, &state->base.allocator.interface);
    IVec2 viewport_size = fw64_renderpass_get_viewport(state->base.renderpass)->size;

    fw64_renderpass_util_ortho2d(state->base.renderpass);

    fw64_spritebatch_begin(state->spritebatch);
    if (message) {
        IVec2 pos = fw64_text_util_center_string(state->base.font, message, &viewport_size);
        pos.y -= path ? fw64_font_line_height(state->base.font) / 2 : 0;
        fw64_spritebatch_draw_string(state->spritebatch, state->base.font, message, pos.x, pos.y);
    }

    if (path) {
        IVec2 pos = fw64_text_util_center_string(state->base.font, path, &viewport_size);
        pos.y += message ? fw64_font_line_height(state->base.font) / 2 : 0;
        fw64_spritebatch_draw_string(state->spritebatch, state->base.font, path, pos.x, pos.y); 
    }
    fw64_spritebatch_end(state->spritebatch);
}

void unknown_state_uninit(fw64AssetViewerUnknownState* state) {
    fw64_spritebatch_delete(state->spritebatch);
}

void unknown_state_update(fw64AssetViewerUnknownState* state) {
    (void)state;
}

void unknown_state_draw(fw64AssetViewerUnknownState* state) {
    fw64_renderpass_draw_sprite_batch(state->base.renderpass, state->spritebatch);
}

int mesh_state_init(fw64AssetViewerMeshState* state, fw64DataSource* data_reader) {
    const fw64Viewport* viewport = fw64_renderpass_get_viewport(state->base.renderpass);
    fw64Allocator* allocator = &state->base.allocator.interface;
    fw64SceneInfo info;
    fw64_scene_info_init(&info);
    info.node_count = 2;
    info.mesh_instance_count = 1;
    fw64_scene_init(&state->scene, &info, state->base.engine->assets, allocator);

    state->mesh = fw64_mesh_load_from_datasource(state->base.engine->assets, data_reader, allocator);
    fw64Node* node = fw64_scene_create_node(&state->scene);
    fw64_scene_create_mesh_instance(&state->scene, node, state->mesh);

    fw64Node* camera_node = fw64_scene_create_node(&state->scene);
    fw64Camera* camera = fw64_allocator_malloc(allocator, sizeof(fw64Camera));
    fw64_camera_init(camera, camera_node, fw64_displays_get_primary(state->base.engine->displays));
    fw64_arcball_init(&state->arcball, state->base.engine->input, camera);
    camera->near = 1.0f;
    camera->far = 300.0f;
    fw64_camera_set_viewport(camera, &viewport->position, &viewport->size);
    fw64_camera_update_projection_matrix(camera);

    Box mesh_bounding = fw64_mesh_get_bounding_box(state->mesh);
    fw64_arcball_set_initial(&state->arcball, &mesh_bounding);

    return 1;
}

void mesh_state_uninit(fw64AssetViewerMeshState* state) {
    if (state->mesh) {
        fw64_mesh_delete(state->mesh, state->base.engine->assets, &state->base.allocator.interface);
    }

     fw64_scene_uninit(&state->scene);   
}

void mesh_state_update(fw64AssetViewerMeshState* state) {
    fw64_arcball_update(&state->arcball, state->base.engine->time->time_delta);
    fw64_renderpass_set_camera(state->base.renderpass, state->arcball.camera);
}

void mesh_state_draw(fw64AssetViewerMeshState* state) {
    fw64Frustum frustum;
    fw64_camera_extract_frustum_planes(state->arcball.camera, &frustum);
    fw64_scene_draw_frustrum(&state->scene, state->base.renderpass, &frustum, ~0U);
}

int text_state_init(fw64AssetViewerTextState* state, fw64DataSource* data_reader) {
    size_t data_size = fw64_data_source_size(data_reader);
    state->text = fw64_bump_allocator_malloc(&state->base.allocator, data_size + 1);

    fw64_data_source_read(data_reader, state->text, 1, data_size);
    state->text[data_size] = 0;

    return 1;
}

static const char* get_extension(const char* path) {
    size_t str_len = strlen(path);
    const char* current = path + str_len;

    while (current != path) {
        if (*current == '.') {
            break;
        }
        current--;
    }

    return current;
}

fw64AssetType fw64_asset_viewer_determine_type(const char* path) {
    const char* extension = get_extension(path);

    if (strcmp(extension, ".mesh") == 0) {
        return FW64_ASSET_TYPE_MESH;
    }

    return FW64_ASSET_TYPE_UNKNOWN;
}

fw64PrimitiveMode fw64_asset_viewer_determine_primitive_mode(fw64AssetViewer* file_viewer) {
    if (file_viewer->state.type != FW64_ASSET_TYPE_MESH) {
        return FW64_PRIMITIVE_MODE_TRIANGLES;
    } else {
        return fw64_mesh_primitive_get_mode(file_viewer->state.mesh.mesh, 0);
    }
}
