#include "lines_example.h"

#define ROTATION_SPEED 90.0f

void lines_example_init(LinesExample* example, Input* input, Renderer* renderer) {
    example->input = input;
    example->renderer = renderer;

    camera_init(&example->camera);
    vec3_set(&example->camera.transform.position, 0.0f, 0.0f, 5.0f);
    Vec3 target = {0.0f, 0.0f, 0.0f};
    Vec3 up = {0.0f, 1.0f, 0.0f};
    transform_look_at(&example->camera.transform, &target, &up);
    camera_update_view_matrix(&example->camera);

    example->draw_mode = EXAMPLE_DRAW_MODE_WIREFRAME_ON_SHADED;

    entity_init(&example->solid_cube);
    entity_init(&example->wire_cube);

    Color clear_color = {255,255,255};
    renderer_set_clear_color(renderer, &clear_color);

    init_consolas(&example->font);

    example->rotation = 0.0f;
}

void lines_example_update(LinesExample* example, float time_delta){
    example->rotation += time_delta * ROTATION_SPEED;

    quat_from_euler(&example->solid_cube.transform.rotation, 0, example->rotation, 0.0f);
    quat_from_euler(&example->wire_cube.transform.rotation, 0, example->rotation, 0.0f);

    if (input_button_pressed(example->input, 0, L_CBUTTONS))
        example->draw_mode = EXAMPLE_DRAW_MODE_WIREFRAME;
    if (input_button_pressed(example->input, 0, U_CBUTTONS))
        example->draw_mode = EXAMPLE_DRAW_MODE_SHADED;
    if (input_button_pressed(example->input, 0, R_CBUTTONS))
        example->draw_mode = EXAMPLE_DRAW_MODE_WIREFRAME_ON_SHADED;
}

static void line_example_draw_wireframe(LinesExample* example) {
    renderer_begin(example->renderer, &example->camera, RENDERER_MODE_LINES,  RENDERER_FLAG_CLEAR);
    wire_object_draw(&example->solid_cube, example->renderer);
    renderer_end(example->renderer, RENDERER_FLAG_SWAP);
}

static void line_example_draw_solid(LinesExample* example) {
    renderer_begin(example->renderer, &example->camera, RENDERER_MODE_TRIANGLES,  RENDERER_FLAG_CLEAR);
    solid_object_draw(&example->wire_cube, example->renderer);
    renderer_end(example->renderer, RENDERER_FLAG_SWAP);
}

static void line_example_draw_wireframe_on_shaded(LinesExample* example) {
    renderer_begin(example->renderer, &example->camera, RENDERER_MODE_TRIANGLES, RENDERER_FLAG_CLEAR);
    solid_object_draw(&example->solid_cube, example->renderer);
    renderer_end(example->renderer, RENDERER_FLAG_NOSWAP);

    renderer_begin(example->renderer, &example->camera, RENDERER_MODE_LINES, RENDERER_FLAG_NOCLEAR);
    wire_object_draw(&example->wire_cube, example->renderer);
    renderer_end(example->renderer, RENDERER_FLAG_SWAP);
}

void lines_example_draw(LinesExample* example) {
    switch (example->draw_mode)
    {
    case EXAMPLE_DRAW_MODE_WIREFRAME:
        line_example_draw_wireframe(example);
        break;

    case EXAMPLE_DRAW_MODE_SHADED:
        line_example_draw_solid(example);
        break;

    case EXAMPLE_DRAW_MODE_WIREFRAME_ON_SHADED:
        line_example_draw_wireframe_on_shaded(example);
        break;
    
    default:
        break;
    }
}
