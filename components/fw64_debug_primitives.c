#include "fw64_debug_primitives.h"

#include "framework64/vec3.h"

typedef struct {
    uint32_t handle;
    fw64Node* base_node;
    fw64Node* tip_node;
    fw64Node* stem_node;
    
} fw64DebugPrimitiveCapsule;

typedef struct {
    uint32_t handle;
    fw64Node* node;
} fw64DebugPrimitive;


void fw64_debug_primitives_init(fw64DebugPrimitives* debug, fw64Engine* engine, const fw64DebugPrimitivesConfig* config, fw64Allocator* allocator) {
    debug->engine = engine;
    debug->primitive_meshes = fw64_assets_load_scene(engine->assets, config->scene_id, allocator);

    debug->config = *config;
    debug->next_handle = 0;

    const size_t node_count = (config->capsule_count * 3) + (config->sphere_count + config->box_count);
    const size_t mesh_instance_count = (config->capsule_count * 3) + (config->sphere_count + config->box_count);

    if (node_count == 0) {
        debug->primitive_meshes = NULL;
        return;
    }

    fw64_static_vector_init(&debug->capsules, sizeof(fw64DebugPrimitiveCapsule), config->capsule_count, allocator);
    fw64_static_vector_init(&debug->spheres, sizeof(fw64DebugPrimitive), config->sphere_count, allocator);
    fw64_static_vector_init(&debug->boxes, sizeof(fw64DebugPrimitive), config->box_count, allocator);

    fw64_static_vector_init(&debug->nodes, sizeof(fw64Node), node_count, allocator);
    fw64_static_vector_init(&debug->mesh_instances, sizeof(fw64MeshInstance), mesh_instance_count, allocator);
}

void fw64_debug_primitives_uninit(fw64DebugPrimitives* debug) {
    if (debug->primitive_meshes == NULL) {
        return;
    }

    fw64_static_vector_uninit(&debug->capsules, debug->primitive_meshes->allocator);
    fw64_static_vector_uninit(&debug->spheres, debug->primitive_meshes->allocator);
    fw64_static_vector_uninit(&debug->boxes, debug->primitive_meshes->allocator);

    fw64_static_vector_uninit(&debug->nodes, debug->primitive_meshes->allocator);
    fw64_static_vector_uninit(&debug->mesh_instances, debug->primitive_meshes->allocator);
    fw64_scene_delete(debug->primitive_meshes);
}

static fw64Node* clone_source_node(fw64DebugPrimitives* debug, fw64NodeIndex node_index) {
    fw64Node* source_node = fw64_scene_get_node(debug->primitive_meshes, node_index);
    fw64Node* cloned_node = fw64_static_vector_alloc_back(&debug->nodes);
    fw64_node_init(cloned_node);
    fw64_mesh_instance_init(fw64_static_vector_alloc_back(&debug->mesh_instances), cloned_node, source_node->mesh_instance->mesh);

    return cloned_node;
}

static void* find_primitive(fw64StaticVector* vec, fw64DebugPrimitiveHandle handle) {
    for (uint32_t i = 0; i < fw64_static_vector_size(vec); i++) {
        fw64DebugPrimitive* primitive = fw64_static_vector_get_item(vec, i);

        if (primitive->handle == handle) {
            return primitive;
        }
    }

    return NULL;
}

static void fw64_debug_primitives_update_capsule_primitive(fw64DebugPrimitives* debug, fw64DebugPrimitiveCapsule* capsule_primtive, const fw64Capsule* capsule) {
    Quat rotation;
    Vec3 capsule_axis, up = vec3_up();
    fw64_capsule_compute_axis(capsule, &capsule_axis);

    quat_ident(&rotation);

    float d = vec3_dot(&up, &capsule_axis) ;
    if (d <= -1 || d >= 1 ){
        quat_ident(&rotation);
    } else {
        Vec3 axis_angle;
        vec3_cross(&up, &capsule_axis, &axis_angle);
        vec3_normalize(&axis_angle);
        float angle_rad = fw64_acosf(d);
        quat_set_axis_angle(&rotation, axis_angle.x, axis_angle.y, axis_angle.z, angle_rad);
    }

    vec3_add(&capsule->a, &capsule->b, &capsule_primtive->stem_node->transform.position);
    capsule_primtive->stem_node->transform.rotation = rotation;
    vec3_scale(&capsule_primtive->stem_node->transform.position, 0.5f, &capsule_primtive->stem_node->transform.position);
    vec3_set(&capsule_primtive->stem_node->transform.scale, debug->config.unit_scale_factor * capsule->radius, debug->config.unit_scale_factor * (fw64_capsule_stem_length(capsule) / 2.0f), debug->config.unit_scale_factor * capsule->radius);
    fw64_node_update(capsule_primtive->stem_node);

    capsule_primtive->base_node->transform.position = capsule->a;
    capsule_primtive->base_node->transform.rotation = rotation;
    vec3_set_all(&capsule_primtive->base_node->transform.scale, debug->config.unit_scale_factor * capsule->radius);
    fw64_node_update(capsule_primtive->base_node);

    capsule_primtive->tip_node->transform.position = capsule->b;
    capsule_primtive->tip_node->transform.rotation = rotation;
    vec3_set_all(&capsule_primtive->tip_node->transform.scale, debug->config.unit_scale_factor * capsule->radius);
    fw64_node_update(capsule_primtive->tip_node);
}

fw64DebugPrimitiveHandle fw64_debug_primitives_add_capsule(fw64DebugPrimitives* debug, const fw64Capsule* capsule){
    fw64DebugPrimitiveCapsule* primitive = fw64_static_vector_alloc_back(&debug->capsules);
    if (!primitive) {
        return FW64_DEBUG_PRIMITIVES_INVALID_HANDLE;
    }

    primitive->handle = debug->next_handle++;

    primitive->stem_node = clone_source_node(debug, debug->config.capsule_stem);
    primitive->base_node = clone_source_node(debug, debug->config.capsule_base);
    primitive->tip_node = clone_source_node(debug, debug->config.capsule_tip);

    fw64_debug_primitives_update_capsule_primitive(debug, primitive, capsule);

    return primitive->handle;
}

void fw64_debug_primitives_update_capsule(fw64DebugPrimitives* debug, fw64DebugPrimitiveHandle handle, const fw64Capsule* capsule) {
    fw64DebugPrimitiveCapsule* primitive = find_primitive(&debug->capsules, handle);

    if (primitive) {
        fw64_debug_primitives_update_capsule_primitive(debug, primitive, capsule);
    }
}

static void fw64_debug_primitives_update_sphere_primitive(fw64DebugPrimitives* debug, fw64DebugPrimitive* sphere_primitive, const Vec3* center, float radius) {
    sphere_primitive->node->transform.position = *center;
    vec3_set_all(&sphere_primitive->node->transform.scale, debug->config.unit_scale_factor * radius);
    fw64_node_update(sphere_primitive->node);
}

fw64DebugPrimitiveHandle fw64_debug_primitives_add_sphere(fw64DebugPrimitives* debug, const Vec3* center, float radius) {
    fw64DebugPrimitive* primitive = fw64_static_vector_alloc_back(&debug->spheres);
    if (!primitive) {
        return FW64_DEBUG_PRIMITIVES_INVALID_HANDLE;
    }

    primitive->handle = debug->next_handle++;
    primitive->node = clone_source_node(debug, debug->config.sphere);

    fw64_debug_primitives_update_sphere_primitive(debug, primitive, center, radius);

    return primitive->handle;
}

void fw64_debug_primitives_update_sphere(fw64DebugPrimitives* debug, fw64DebugPrimitiveHandle handle, const Vec3* center, float radius) {
    fw64DebugPrimitive* primitive = find_primitive(&debug->boxes, handle);

    if (primitive) {
        fw64_debug_primitives_update_sphere_primitive(debug, primitive, center, radius);
    }
}

static void fw64_debug_primitives_update_box_primitive(fw64DebugPrimitives* debug, fw64DebugPrimitive* box_primitive, const Box* box) {
    box_center(box, &box_primitive->node->transform.position);
    box_extents(box, &box_primitive->node->transform.scale);
    vec3_scale(&box_primitive->node->transform.scale, debug->config.unit_scale_factor, &box_primitive->node->transform.scale);
    fw64_node_update(box_primitive->node);
}

fw64DebugPrimitiveHandle fw64_debug_primitives_add_box(fw64DebugPrimitives* debug, const Box* box) {
    fw64DebugPrimitive* primitive = fw64_static_vector_alloc_back(&debug->boxes);
    if (!primitive) {
        return FW64_DEBUG_PRIMITIVES_INVALID_HANDLE;
    }

    primitive->handle = debug->next_handle++;
    primitive->node = clone_source_node(debug, debug->config.box);

    fw64_debug_primitives_update_box_primitive(debug, primitive, box);

    return primitive->handle;
}

void fw64_debug_primitives_update_box(fw64DebugPrimitives* debug, fw64DebugPrimitiveHandle handle, const Box* box) {
    fw64DebugPrimitive* primitive = find_primitive(&debug->boxes, handle);

    if (primitive) {
        fw64_debug_primitives_update_box_primitive(debug, primitive, box);
    }
}

void fw64_debug_primitives_draw(fw64DebugPrimitives* debug, fw64RenderPass* renderpass) {
    for (uint32_t i = 0; i < fw64_static_vector_size(&debug->capsules); i++) {
        fw64DebugPrimitiveCapsule* capsule = fw64_static_vector_get_item(&debug->capsules, i);

        fw64_renderpass_draw_static_mesh(renderpass, capsule->stem_node->mesh_instance);
        fw64_renderpass_draw_static_mesh(renderpass, capsule->base_node->mesh_instance);
        fw64_renderpass_draw_static_mesh(renderpass, capsule->tip_node->mesh_instance);
    }

    for (uint32_t i = 0; i < fw64_static_vector_size(&debug->spheres); i++) {
        fw64DebugPrimitive* sphere = fw64_static_vector_get_item(&debug->spheres, i);
        fw64_renderpass_draw_static_mesh(renderpass, sphere->node->mesh_instance);
    }

    for (uint32_t i = 0; i < fw64_static_vector_size(&debug->boxes); i++) {
        fw64DebugPrimitive* box = fw64_static_vector_get_item(&debug->boxes, i);
        fw64_renderpass_draw_static_mesh(renderpass, box->node->mesh_instance);
    }
}
