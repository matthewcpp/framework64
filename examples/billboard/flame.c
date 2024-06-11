#include "flame.h"
#include "framework64/util/quad.h"

#include "assets/assets.h"

#define FLAME_UPDATE_TIME 1.0f / 15.0f

void flame_init(Flame* flame, fw64Engine* engine, fw64Node* node){
    fw64Allocator* allocator = fw64_default_allocator();

    flame->entity = node;

    fw64Image* flame_image = fw64_assets_load_image_dma(engine->assets, FW64_ASSET_image_fire_sprite, allocator);
    flame->texture = fw64_texture_create_from_image(flame_image, allocator);

    fw64TexturedQuadParams params;
    fw64_textured_quad_params_init(&params);
    params.texture = flame->texture;
    params.is_animated = 1;
    fw64Mesh* quad = fw64_textured_quad_create_with_params(engine, &params, allocator);

    fw64_node_init(node);
    fw64_node_set_mesh(node, quad);
    vec3_set(&node->transform.scale, 10.0f, 12.0f, 10.0f);
    flame->update_time_remaining = FLAME_UPDATE_TIME;
}

void flame_update(Flame* flame, float time_delta) {
    flame->update_time_remaining -= time_delta;

    if (flame->update_time_remaining <= 0.0f) {
        flame->update_time_remaining += FLAME_UPDATE_TIME;

        fw64Material* material = fw64_mesh_get_material_for_primitive(flame->entity->mesh, 0);
        fw64Texture* texture = fw64_material_get_texture(material);
        uint32_t frameCount = fw64_texture_hslices(texture) * fw64_texture_vslices(texture);
        uint32_t current_tex_frame = fw64_material_get_texture_frame(material);
        fw64_material_set_texture_frame(material, (current_tex_frame + 1) % frameCount);
    }
}
