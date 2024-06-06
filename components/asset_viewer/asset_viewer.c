#include "asset_viewer.h"

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

void fw64_file_viewer_init(fw64AssetViewer* file_viewer, fw64Engine* engine, fw64Font* font, uint32_t data_size) {
    file_viewer->state.base.engine = engine;
    file_viewer->state.base.font = font;
    
    fw64_bump_allocator_init(&file_viewer->state.base.allocator, data_size);

    file_viewer->state.type = FW64_ASSET_TYPE_UNKNOWN;
    unknown_state_init(&file_viewer->state.unknown, "No Asset Loaded", NULL);
}

void fw64_file_viewer_uninit(fw64AssetViewer* file_viewer) {
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

void fw64_file_viewer_set_empty_state(fw64AssetViewer* file_viewer, const char* message, const char* path) {
    uninit_current_state(file_viewer);
    file_viewer->state.type = FW64_ASSET_TYPE_UNKNOWN;
    unknown_state_init(&file_viewer->state.unknown, message, path);
}

void fw64_file_viewer_update(fw64AssetViewer* file_viewer) {
    switch (file_viewer->state.type) {
        case FW64_ASSET_TYPE_UNKNOWN:
            unknown_state_update(&file_viewer->state.unknown);
            break;

        case FW64_ASSET_TYPE_MESH:
            mesh_state_update(&file_viewer->state.mesh);
            break;
    }
}

void fw64_file_viewer_draw(fw64AssetViewer* file_viewer) {
    switch (file_viewer->state.type) {
        case FW64_ASSET_TYPE_UNKNOWN:
            unknown_state_draw(&file_viewer->state.unknown);
            break;

        case FW64_ASSET_TYPE_MESH:
            mesh_state_draw(&file_viewer->state.mesh);
            break;
    }
}

void fw64_file_viewer_load_from_media(fw64AssetViewer* file_viewer, const char* path) {
    fw64AssetType type = fw64_file_viewer_determine_type(path);

    if (type == FW64_ASSET_TYPE_UNKNOWN) {
        fw64_file_viewer_set_empty_state(file_viewer, "Unknown asset type:", path);
        return;
    }

    fw64Media* media = file_viewer->state.base.engine->media;
    fw64DataSource* data_reader = fw64_media_open_data_source(media, path);

    if (!data_reader) {
        fw64_file_viewer_set_empty_state(file_viewer, "File open failed:", path);
        return;
    }
    fw64_file_viewer_load_data(file_viewer, data_reader, type);
    fw64_media_close_data_source(media, data_reader);
}

void fw64_file_viewer_load_data(fw64AssetViewer* file_viewer, fw64DataSource* data_reader, fw64AssetType type) {
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
        fw64_file_viewer_set_empty_state(file_viewer, "Asset loading failed", NULL);
    }
}

void unknown_state_init(fw64AssetViewerUnknownState* state, const char* message, const char* path) {
    state->message = NULL;
    state->path = NULL;

    if (message) {
        size_t message_len = strlen(message);
        state->message = fw64_bump_allocator_malloc(&state->base.allocator, message_len + 1);
        strcpy(state->message, message);
    }

    if (path) {
        size_t path_len = strlen(path);
        state->path = fw64_bump_allocator_malloc(&state->base.allocator, path_len + 1);
        strcpy(state->path, path);
    }
}

void unknown_state_uninit(fw64AssetViewerUnknownState* state) {
    (void)state;
}

void unknown_state_update(fw64AssetViewerUnknownState* state) {
    (void)state;
}

void unknown_state_draw(fw64AssetViewerUnknownState* state) {
    fw64Renderer* renderer = state->base.engine->renderer;
    fw64_renderer_begin(renderer, FW64_PRIMITIVE_MODE_TRIANGLES, FW64_RENDERER_FLAG_CLEAR);
    int y_pos = 10;
    fw64_renderer_draw_text(state->base.engine->renderer, state->base.font, 10, y_pos, state->message);

    y_pos += fw64_font_size(state->base.font) + 4;
    fw64_renderer_draw_text(state->base.engine->renderer, state->base.font, 10, y_pos, state->path);

    fw64_renderer_end(renderer, FW64_RENDERER_FLAG_SWAP);
}

int mesh_state_init(fw64AssetViewerMeshState* state, fw64DataSource* data_reader) {
    state->node = fw64_bump_allocator_malloc(&state->base.allocator, sizeof(fw64Node));
    fw64_node_init(state->node);

    fw64Collider* collider = fw64_bump_allocator_malloc(&state->base.allocator, sizeof(fw64Collider));

    fw64Mesh* mesh = fw64_mesh_load_from_datasource(state->base.engine->assets, data_reader, &state->base.allocator.interface);

    Box bounding_box = fw64_mesh_get_bounding_box(mesh);

    Vec3 extents;
    box_extents(&bounding_box, &extents);
    float max_extent = extents.x;
    max_extent = extents.y > max_extent ? extents.y : max_extent;
    max_extent = extents.z > max_extent ? extents.z : max_extent;

    float scale = 20.0f / max_extent;

    vec3_set_all(&state->node->transform.scale, scale);
    fw64_node_set_mesh(state->node, mesh);
    fw64_node_set_box_collider(state->node, collider);
    fw64_node_update(state->node);

    fw64_arcball_init(&state->arcball, state->base.engine->input, fw64_displays_get_primary(state->base.engine->displays));
    state->arcball.camera.near = 1.0f;
    state->arcball.camera.far = 100.0f;
    fw64_camera_update_view_matrix(&state->arcball.camera);

    fw64_arcball_set_initial(&state->arcball, &state->node->collider->bounding);

    return 1;
}

void mesh_state_uninit(fw64AssetViewerMeshState* state) {
    if (state->node->mesh)
        fw64_mesh_delete(state->node->mesh, state->base.engine->assets, &state->base.allocator.interface);
}

void mesh_state_update(fw64AssetViewerMeshState* state) {
    fw64_arcball_update(&state->arcball, state->base.engine->time->time_delta);
}

void mesh_state_draw(fw64AssetViewerMeshState* state) {
    fw64Renderer* renderer = state->base.engine->renderer;

    fw64_renderer_begin(renderer, FW64_PRIMITIVE_MODE_TRIANGLES, FW64_RENDERER_FLAG_CLEAR);
    fw64_renderer_set_camera(renderer, &state->arcball.camera);
    fw64_renderer_draw_static_mesh(renderer, &state->node->transform, state->node->mesh);
    fw64_renderer_end(renderer, FW64_RENDERER_FLAG_SWAP);
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

fw64AssetType fw64_file_viewer_determine_type(const char* path) {
    const char* extension = get_extension(path);

    if (strcmp(extension, ".mesh") == 0) {
        return FW64_ASSET_TYPE_MESH;
    }

    return FW64_ASSET_TYPE_UNKNOWN;
}
