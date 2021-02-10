#include "billboard_example.h"

#include "assets.h"

#include <math.h>

void lines_example_init(BillboardExample* example, Input* input, Renderer* renderer) {
    example->input = input;
    example->renderer = renderer;


    fps_camera_init(&example->fps, input);
    vec3_set(&example->fps.camera.transform.position, 0.0f, 4.0f, 20.0f);

    entity_init(&example->solid_cube);

    sprite_load(ASSET_sprite_nintendo_seal, &example->nintendo_seal_sprite);
    billboard_quad_init(&example->nintendo_seal_quad, BILLBOARD_QUAD_1X1, &example->nintendo_seal_sprite);
    vec3_set(&example->nintendo_seal_quad.transform.position, -3.0f, 5.0f, 0.0f);

    sprite_load(ASSET_sprite_n64_logo, &example->n64_logo_sprite);
    billboard_quad_init(&example->n64_logo_quad, BILLBOARD_QUAD_2X2, &example->n64_logo_sprite);
    vec3_set(&example->n64_logo_quad.transform.position, 3.0f, 5.0f, 0.0f);
}

void lines_example_update(BillboardExample* example, float time_delta){
    fps_camera_update(&example->fps, time_delta);
}

void lines_example_draw(BillboardExample* example) {
    renderer_begin(example->renderer, &example->fps.camera, RENDERER_MODE_TRIANGLES, RENDERER_FLAG_CLEAR);
    solid_object_draw(&example->solid_cube, example->renderer);
    renderer_draw_billboard_quad(example->renderer, &example->nintendo_seal_quad);
    renderer_draw_billboard_quad(example->renderer, &example->n64_logo_quad);
    renderer_end(example->renderer, RENDERER_FLAG_SWAP);
}
