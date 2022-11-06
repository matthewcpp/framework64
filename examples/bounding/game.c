#include "game.h"
#include "assets/assets.h"

#include "framework64/matrix.h"
#include "framework64/n64/controller_button.h"

#include <math.h>
#include <stdio.h>

#define PENGUIN_SIZE_CHANGE_SPEED 0.15f
#define PENGUIN_MOVE_SPEED 85.0f
#define STICK_THRESHOLD 0.15f

static void setup_camera(Game* game);
static void init_cubes(Game* game);
static int check_intersection(Game* game);
static const char* intersection_text(CubeId cube);

float stick_adjust[4];

void game_init(Game* game, fw64Engine* engine) {
    game->engine = engine;

    fw64_node_init(&game->penguin);
    fw64_node_set_mesh(&game->penguin, fw64_mesh_load(engine->assets, FW64_ASSET_mesh_penguin, NULL));
    fw64_node_set_box_collider(&game->penguin, &game->penguin_collider);
    vec3_set(&game->penguin.transform.scale, 0.1f, 0.1f, 0.1f);
    quat_set_axis_angle(&game->penguin.transform.rotation, 0, 1, 0, M_PI);
    fw64_node_update(&game->penguin);

    fw64_node_init(&game->penguin_box);
    fw64_node_set_mesh(&game->penguin_box, fw64_mesh_load(engine->assets, FW64_ASSET_mesh_blue_cube_wire, NULL));
    game->font = fw64_font_load(engine->assets, FW64_ASSET_font_Consolas12, NULL);
    game->intersection = CUBE_NONE;

    setup_camera(game);

    mat2_set_rotation(stick_adjust, M_PI / 2.0f);
    init_cubes(game);
}

void game_update(Game* game){
    Vec2 stick;
    fw64_input_controller_stick(game->engine->input, 0, &stick);
    
    if (stick.x >= STICK_THRESHOLD || stick.x <= -STICK_THRESHOLD  || stick.y >= STICK_THRESHOLD || stick.y <= -STICK_THRESHOLD) {
        mat2_transform_vec2(stick_adjust, & stick);
        float orientation = atan2f(stick.y, stick.x);
        quat_set_axis_angle(&game->penguin.transform.rotation, 0, 1, 0, orientation);

        Vec3 forward;
        fw64_transform_forward(&game->penguin.transform, &forward);
        vec3_scale(&forward, &forward, PENGUIN_MOVE_SPEED * game->engine->time->time_delta);
        vec3_add(&game->penguin.transform.position, &game->penguin.transform.position, &forward);
    }

    Vec3 scale_delta;
    vec3_set_all(&scale_delta, game->engine->time->time_delta * PENGUIN_SIZE_CHANGE_SPEED);
    if (fw64_input_controller_button_down(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_C_UP)) {
        vec3_add(&game->penguin.transform.scale, &game->penguin.transform.scale, &scale_delta);
    }

    if (fw64_input_controller_button_down(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_C_DOWN)) {
        vec3_negate(&scale_delta);
        vec3_add(&game->penguin.transform.scale, &game->penguin.transform.scale, &scale_delta);
    }

    fw64_node_update(&game->penguin);

    box_center(&game->penguin.collider->bounding, &game->penguin_box.transform.position);
    box_extents(&game->penguin.collider->bounding, &game->penguin_box.transform.scale);

    fw64_node_update(&game->penguin_box);

    game->intersection = check_intersection(game);
}

void game_draw(Game* game) {
    fw64Renderer* renderer = game->engine->renderer;
    char text_buffer[64];
    sprintf(text_buffer, "Intersection: %s", intersection_text(game->intersection));

    fw64_renderer_set_anti_aliasing_enabled(renderer, 1);

    fw64_renderer_begin(renderer, FW64_PRIMITIVE_MODE_TRIANGLES, FW64_RENDERER_FLAG_CLEAR);
    fw64_renderer_set_camera(renderer, &game->camera);
    
    fw64_renderer_draw_static_mesh(renderer, &game->penguin.transform, game->penguin.mesh);

    for (int i = 0; i < CUBE_COUNT; i++){
        fw64_renderer_draw_static_mesh(renderer, &game->cubes[i].transform, game->cubes[i].mesh);
    }

    fw64_renderer_set_anti_aliasing_enabled(renderer, 0);

    fw64_renderer_draw_text(renderer, game->font, 10, 10, text_buffer);

    fw64_renderer_end(renderer, FW64_RENDERER_FLAG_NOSWAP);

    fw64_renderer_begin(renderer, FW64_PRIMITIVE_MODE_LINES, FW64_RENDERER_FLAG_NOCLEAR);
    fw64_renderer_set_camera(renderer, &game->camera);
    fw64_renderer_draw_static_mesh(renderer, &game->penguin_box.transform, game->penguin_box.mesh);
    fw64_renderer_end(renderer, FW64_RENDERER_FLAG_SWAP);
}

void setup_camera(Game* game) {
    fw64_camera_init(&game->camera);

    game->camera.near = 10.0f;
    game->camera.far = 500.0f;
    fw64_camera_update_projection_matrix(&game->camera);

    Vec3 center = {0.0f, 0.0f, 0.0f}, up = {0.0f, 1.0f, 0.0f};

    vec3_set(&game->camera.transform.position, 0.0f, 100.0f, -125.0f);
    fw64_transform_look_at(&game->camera.transform, &center, &up);

    fw64_camera_update_view_matrix(&game->camera);
}

Vec3 cube_positions[CUBE_COUNT] = {
    {-50.0f, 5.0f, 50.0f},
    {50.0f, 5.0f, 50.0f},
    {-50.0f, 5.0f, -50.0f},
    {50.0f, 5.0f, -50.0f},
};

void init_cubes(Game* game) {
    
    fw64Mesh* cube_mesh = fw64_mesh_load(game->engine->assets, FW64_ASSET_mesh_blue_cube, NULL);

    for (int i = 0; i < CUBE_COUNT; i++) {
        fw64Node* cube = &game->cubes[i];
        fw64_node_init(cube);
        fw64_node_set_mesh(&game->cubes[i], cube_mesh);
        fw64_node_set_box_collider(cube, &game->cube_colliders[i]);
        cube->transform.position = cube_positions[i];
        vec3_set_all(&cube->transform.scale, 10.0f);
        fw64_node_update(cube);
    }
}

static int check_intersection(Game* game) {
    for (int i = 0; i < CUBE_COUNT; i++) {
        if (fw64_collider_test_box(game->penguin.collider, &game->cubes[i].collider->bounding)) {
            return i;
        }
    }

    return CUBE_COUNT;
}

const char* intersection_text(CubeId cube) {
    switch (cube)
    {
        case CUBE_TOP_LEFT:
            return "Top Left";
        case CUBE_TOP_RIGHT:
            return "Top Right";
        case CUBE_BOTTOM_LEFT:
            return "Bottom Left";
        case CUBE_BOTTOM_RIGHT:
            return "Bottom Right";
        default:
            return "None";
    }
}