#include "billboard_example.h"

#include <math.h>

void lines_example_init(BillboardExample* example, Input* input, Renderer* renderer) {
    example->input = input;
    example->renderer = renderer;


    fps_camera_init(&example->fps, input);
    vec3_set(&example->fps.camera.transform.position, 0.0f, 4.0f, 20.0f);

    entity_init(&example->solid_cube);
    init_sprite(&example->sprite);
    billboard_quad_init(&example->quad, &example->sprite, 0);
    example->quad.transform.position.y = 5;
}

void lines_example_update(BillboardExample* example, float time_delta){
    fps_camera_update(&example->fps, time_delta);
}

void lines_example_draw(BillboardExample* example) {
    renderer_begin(example->renderer, &example->fps.camera, RENDERER_MODE_TRIANGLES, RENDERER_FLAG_CLEAR);
    solid_object_draw(&example->solid_cube, example->renderer);
    renderer_draw_billboard_quad(example->renderer, &example->quad);
    renderer_end(example->renderer, RENDERER_FLAG_SWAP);
}
