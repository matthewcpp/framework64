#pragma once

#include <framework64/engine.h>
#include <framework64/scene.h>
#include <framework64/static_vector.h>

#include <limits.h>

#define FW64_DEBUG_PRIMITIVES_INVALID_HANDLE UINT32_MAX

typedef uint32_t fw64DebugPrimitiveHandle;

typedef struct {
    fw64AssetId scene_id;
    fw64NodeIndex capsule_stem;
    fw64NodeIndex capsule_base;
    fw64NodeIndex capsule_tip;
    fw64NodeIndex sphere;
    fw64NodeIndex box;
    /** This scale factor brings the primitive geometry to a unit factor */
    float unit_scale_factor;
    uint32_t box_count;
    uint32_t sphere_count;
    uint32_t capsule_count;
} fw64DebugPrimitivesConfig;

typedef struct {
    fw64Engine* engine;
    fw64Scene* primitive_meshes;
    fw64DebugPrimitivesConfig config;

    fw64StaticVector nodes;
    fw64StaticVector mesh_instances;
    fw64StaticVector capsules;
    fw64StaticVector spheres;
    fw64StaticVector boxes;
    fw64DebugPrimitiveHandle next_handle;
} fw64DebugPrimitives;

#ifdef __cplusplus
extern "C" {
#endif

void fw64_debug_primitives_init(fw64DebugPrimitives* debug, fw64Engine* engine, const fw64DebugPrimitivesConfig* config, fw64Allocator* allocator);
void fw64_debug_primitives_uninit(fw64DebugPrimitives* debug);

fw64DebugPrimitiveHandle fw64_debug_primitives_add_capsule(fw64DebugPrimitives* debug, const fw64Capsule* capsule);
fw64DebugPrimitiveHandle fw64_debug_primitives_add_sphere(fw64DebugPrimitives* debug, const Vec3* center, float radius);
fw64DebugPrimitiveHandle fw64_debug_primitives_add_box(fw64DebugPrimitives* debug, const Box* box);

void fw64_debug_primitives_update_capsule(fw64DebugPrimitives* debug, fw64DebugPrimitiveHandle handle, const fw64Capsule* capsule);
void fw64_debug_primitives_update_sphere(fw64DebugPrimitives* debug, fw64DebugPrimitiveHandle handle, const Vec3* center, float radius);
void fw64_debug_primitives_update_box(fw64DebugPrimitives* debug, fw64DebugPrimitiveHandle handle, const Box* box);

void fw64_debug_primitives_draw(fw64DebugPrimitives* debug, fw64RenderPass* renderpass);

#ifdef __cplusplus
}
#endif
