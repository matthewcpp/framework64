#include "game.h"
#include "assets.h"

#include "framework64/asset_database.h"
#include "framework64/matrix.h"
#include "framework64/n64/controller_button.h"

#include <math.h>
#include <stdio.h>

#define PENGUIN_SIZE_CHANGE_SPEED 0.25f
#define PENGUIN_MOVE_SPEED 350.0f
#define STICK_THRESHOLD 0.15f

static void setup_camera(Game* game);
static void init_cubes(Game* game);
static int check_intersection(Game* game);
static const char* intersection_text(CubeId cube);

float stick_adjust[4];

void game_init(Game* game, fw64Engine* engine) {
    game->engine = engine;
    
    entity_init(&game->penguin, fw64_assets_get_mesh(engine->assets, FW64_ASSET_mesh_penguin));
    entity_init(&game->penguin_box, fw64_assets_get_mesh(engine->assets, FW64_ASSET_mesh_blue_cube_wire));
    game->font = fw64_assets_get_font(engine->assets, FW64_ASSET_font_Consolas12);
    game->intersection = CUBE_NONE;

    vec3_set(&game->penguin.transform.scale, 0.5, 0.5, 0.5);
    quat_set_axis_angle(&game->penguin.transform.rotation, 0, 1, 0, M_PI);

    setup_camera(game);

    mat2_set_rotation(stick_adjust, M_PI / 2.0f);
    init_cubes(game);
}

void game_update(Game* game){
    Vec2 stick;
    fw64_input_stick(game->engine->input, 0, &stick);
    
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
    if (fw64_input_button_down(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_C_UP)) {
        vec3_add(&game->penguin.transform.scale, &game->penguin.transform.scale, &scale_delta);
    }

    if (fw64_input_button_down(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_C_DOWN)) {
        vec3_negate(&scale_delta);
        vec3_add(&game->penguin.transform.scale, &game->penguin.transform.scale, &scale_delta);
    }

    entity_refresh(&game->penguin);

    box_center(&game->penguin.bounding, &game->penguin_box.transform.position);
    box_extents(&game->penguin.bounding, &game->penguin_box.transform.scale);

    entity_refresh(&game->penguin_box);

    game->intersection = check_intersection(game);
}

void game_draw(Game* game) {
    char text_buffer[64];
    sprintf(text_buffer, "Intersection: %s", intersection_text(game->intersection));

    fw64_renderer_begin(game->engine->renderer, &game->camera, FW64_RENDERER_MODE_TRIANGLES, FW64_RENDERER_FLAG_CLEAR);
    
    fw64_renderer_draw_static_mesh(game->engine->renderer, &game->penguin.transform, game->penguin.mesh);

    for (int i = 0; i < CUBE_COUNT; i++){
        fw64_renderer_draw_static_mesh(game->engine->renderer, &game->cubes[i].transform, game->cubes[i].mesh);
    }

    fw64_renderer_end(game->engine->renderer, FW64_RENDERER_FLAG_NOSWAP);

    fw64_renderer_begin(game->engine->renderer, &game->camera, FW64_RENDERER_MODE_LINES, FW64_RENDERER_FLAG_NOCLEAR);
    fw64_renderer_draw_static_mesh(game->engine->renderer, &game->penguin_box.transform, game->penguin_box.mesh);
    fw64_renderer_end(game->engine->renderer, FW64_RENDERER_FLAG_NOSWAP);

    fw64_renderer_begin(game->engine->renderer, &game->camera, FW64_RENDERER_MODE_ORTHO2D, FW64_RENDERER_FLAG_NOCLEAR);
    fw64_renderer_draw_text(game->engine->renderer, game->font, 10, 10, text_buffer);
    fw64_renderer_end(game->engine->renderer, FW64_RENDERER_FLAG_SWAP);
}

void setup_camera(Game* game) {
    fw64_camera_init(&game->camera);

    game->camera.near = 250.0f;
    game->camera.far = 1000;
    fw64_camera_update_projection_matrix(&game->camera);

    Vec3 center, size, up = {0.0f, 1.0f, 0.0f};

    box_center(&game->penguin.bounding, &center);
    box_size(&game->penguin.bounding, &size);

    vec3_set(&game->camera.transform.position, center.x, center.y + size.y * 2, center.z - size.z* 2);
    fw64_transform_look_at(&game->camera.transform, &center, &up);

    fw64_camera_update_view_matrix(&game->camera);
}

Vec3 cube_positions[CUBE_COUNT] = {
    {-200, 0, 200},
    {200, 0, 200},
    {-200, 0, -200},
    {200, 0, -200},
};

void init_cubes(Game* game) {
    
    fw64Mesh* cube_mesh = fw64_assets_get_mesh(game->engine->assets, FW64_ASSET_mesh_blue_cube);

    for (int i = 0; i < CUBE_COUNT; i++) {
        Entity* cube = &game->cubes[i];
        entity_init(cube, cube_mesh);
        cube->transform.position = cube_positions[i];
        vec3_set(&cube->transform.scale, 40, 40, 40);
        entity_refresh(cube);
    }
}

static int check_intersection(Game* game) {
    for (int i = 0; i < CUBE_COUNT; i++) {
        if (box_intersection(&game->penguin.bounding, &game->cubes[i].bounding))
            return i;
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