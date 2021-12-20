#include "flame.h"
#include "framework64/util/quad.h"

#include "assets.h"

#define FLAME_UPDATE_TIME 1.0f / 15.0f

void flame_init(Flame* flame, fw64Engine* engine){
    fw64Image* flame_image = fw64_image_load_with_options(engine->assets, FW64_ASSET_image_fire_sprite, FW64_IMAGE_FLAG_DMA_MODE, NULL);

    fw64TexturedQuadParams params;
    fw64_textured_quad_params_init(&params);
    params.image = flame_image;
    params.is_animated = 1;
    fw64Mesh* quad = fw64_textured_quad_create_with_params(engine, &params, NULL);

    fw64_node_init(&flame->entity);
    fw64_node_set_mesh(&flame->entity, quad);
    flame->update_time_remaining = FLAME_UPDATE_TIME;
}

void flame_update(Flame* flame, float time_delta) {
    flame->update_time_remaining -= time_delta;

    if (flame->update_time_remaining <= 0.0f) {
        flame->update_time_remaining += FLAME_UPDATE_TIME;

        fw64Material* material = fw64_mesh_get_material_for_primitive(flame->entity.mesh, 0);
        fw64Texture* texture = fw64_material_get_texture(material);
        int frameCount = fw64_texture_hslices(texture) * fw64_texture_vslices(texture);
        int current_tex_frame = fw64_material_get_texture_frame(material);
        fw64_material_set_texture_frame(material, (current_tex_frame + 1) % frameCount);
    }
}

void flame_draw(Flame* flame, fw64Renderer* renderer) {
    fw64_node_billboard(&flame->entity, fw64_renderer_get_camera(renderer));

    fw64_renderer_set_depth_testing_enabled(renderer, 0);
    fw64_renderer_draw_static_mesh(renderer, &flame->entity.transform, flame->entity.mesh);
    fw64_renderer_set_depth_testing_enabled(renderer, 1);
}