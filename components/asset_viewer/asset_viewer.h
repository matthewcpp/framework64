#pragma once

#include <framework64/data_io.h>
#include <framework64/engine.h>
#include <framework64/scene.h>

#include <framework64/util/bump_allocator.h>
#include "arcball_camera/arcball_camera.h"

typedef enum {
    FW64_ASSET_TYPE_UNKNOWN,
    FW64_ASSET_TYPE_MESH,
} fw64AssetType;

typedef struct {
    fw64AssetType type;
    fw64Engine* engine;
    fw64Font* font;
    fw64RenderPass* renderpass;
    fw64BumpAllocator allocator;
} fw64AssetViewerStateBase;

typedef struct {
    fw64AssetViewerStateBase base;
    fw64SpriteBatch* spritebatch;
} fw64AssetViewerUnknownState;

typedef struct {
    fw64AssetViewerStateBase base;
    fw64Mesh* mesh;
    fw64Scene scene;
    fw64ArcballCamera arcball;
} fw64AssetViewerMeshState;

typedef struct {
    fw64AssetViewerStateBase base;
    char* text;
} fw64AssetViewerTextState;

typedef union {
    fw64AssetType type;
    fw64AssetViewerStateBase base;
    fw64AssetViewerUnknownState unknown;
    fw64AssetViewerMeshState mesh;
    fw64AssetViewerTextState text;
}  fw64AssetViewerStateData;

typedef struct {    
    fw64AssetViewerStateData state;
} fw64AssetViewer;

#ifdef __cplusplus
extern "C" {
#endif

void fw64_asset_viewer_init(fw64AssetViewer* file_viewer, fw64Engine* engine, fw64RenderPass* renderpass, fw64Font* font, uint32_t data_size);
void fw64_asset_viewer_uninit(fw64AssetViewer* file_viewer);
void fw64_asset_viewer_update(fw64AssetViewer* file_viewer);
void fw64_asset_viewer_draw(fw64AssetViewer* file_viewer);

fw64AssetType fw64_asset_viewer_determine_type(const char* path);
fw64PrimitiveMode fw64_asset_viewer_determine_primitive_mode(fw64AssetViewer* file_viewer);

void fw64_asset_viewer_load_from_media(fw64AssetViewer* file_viewer, const char* path);
void fw64_asset_viewer_load_data(fw64AssetViewer* file_viewer, fw64DataSource* data_reader, fw64AssetType type);

void fw64_asset_viewer_set_empty_state(fw64AssetViewer* file_viewer, const char* message, const char* path);

#ifdef __cplusplus
}
#endif
