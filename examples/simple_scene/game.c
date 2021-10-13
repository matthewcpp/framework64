#include "game.h"
#include "assets.h"
#include "framework64/matrix.h"
#include "framework64/n64/controller_button.h"

static void init_scene(Game* game);
static void update_camera(Game* game);
static void control_player(Game* game);
float rotation = 0.0f;

void game_init(Game* game, fw64Engine* engine) {
    game->engine = engine;
    fw64_camera_init(&game->camera);
    game->camera.near = 1.0f;
    game->camera.far = 100.0f;
    fw64_camera_update_projection_matrix(&game->camera);

    init_scene(game);
}

void game_update(Game* game){
    control_player(game);
    update_camera(game);
    flame_update(&game->flames[0], game->engine->time->time_delta);
    flame_update(&game->flames[1], game->engine->time->time_delta);
}

void game_draw(Game* game) {
    fw64Renderer* renderer = game->engine->renderer;

    fw64_renderer_begin(renderer, &game->camera, FW64_RENDERER_MODE_TRIANGLES, FW64_RENDERER_FLAG_CLEAR);
    fw64_scene_draw_all(game->scene, renderer);
    fw64_renderer_draw_static_mesh(renderer, &game->player.transform, game->player.mesh);
    flame_draw(&game->flames[0], renderer);
    flame_draw(&game->flames[1], renderer);
    fw64_renderer_end(renderer, FW64_RENDERER_FLAG_SWAP);
}

#define EXTRA_TYPE_START 0
#define EXTRA_TYPE_FIRE 1

void init_scene(Game* game) {
    game->scene = fw64_scene_load(game->engine->assets, FW64_ASSET_scene_simple_scene);

    int flame_index = 0;
    fw64Image* flame_image = fw64_image_load(game->engine->assets, FW64_ASSET_image_fire_sprite);

    fw64SceneExtra* extras = fw64_scene_get_extras(game->scene);
    uint32_t extras_count = fw64_scene_get_extra_count(game->scene);

    for (uint32_t i = 0; i < extras_count; i++) {
        fw64SceneExtra* extra = extras + i;

        if (extra->type == EXTRA_TYPE_START) {
            fw64_node_init(&game->player, fw64_mesh_load(game->engine->assets, FW64_ASSET_mesh_penguin));
            vec3_set(&game->player.transform.scale, 0.01f, 0.01f, 0.01f);
            game->player.transform.position = extra->position;
            fw64_node_refresh(&game->player);
        }
        else if (extra->type == EXTRA_TYPE_FIRE) {
            Flame* flame = &game->flames[flame_index++];
            flame_init(flame, game->engine, flame_image);
            vec3_set(&flame->entity.transform.scale, 3.0f, 5.0f, 3.0f);
            flame->entity.transform.position = extra->position;
            flame->entity.transform.position.y += 3.0f;
        }
    }
}

#define CAMERA_BACK_FOLLOW_DIST 9.0f
#define CAMERA_FORWARD_TARGET_DIST 5.0f
#define CAMERA_FOLLOW_HEIGHT 10.0f

void update_camera(Game* game) {
    Vec3 back, forward, camera_pos, camera_target;
    Vec3 up = {0.0, 1.0, 0.0};
    fw64_transform_back(&game->player.transform, &back);
    vec3_copy(&forward, &back);
    vec3_negate(&forward);

    vec3_scale(&back, &back, CAMERA_BACK_FOLLOW_DIST);
    vec3_add(&camera_pos, &game->player.transform.position, &back);
    camera_pos.y += CAMERA_FOLLOW_HEIGHT;

    vec3_scale(&forward, &forward, CAMERA_FORWARD_TARGET_DIST);
    vec3_add(&camera_target, &game->player.transform.position, &forward);

    game->camera.transform.position = camera_pos;
    fw64_transform_look_at(&game->camera.transform, &camera_target, &up);
    fw64_camera_update_view_matrix(&game->camera);
}

#define PLAYER_MOVE_SPEED 10.0f
#define PLAYER_ROTATION_SPEED 45.0f
#define STICK_THRESHOLD 0.15f

void control_player(Game* game) {
    Vec2 stick;
    fw64_input_stick(game->engine->input, 0, &stick);
    if (stick.x >= STICK_THRESHOLD || stick.x <= -STICK_THRESHOLD  || stick.y >= STICK_THRESHOLD || stick.y <= -STICK_THRESHOLD) {
        float rotation_delta = PLAYER_ROTATION_SPEED * game->engine->time->time_delta;

        if (stick.x >= STICK_THRESHOLD) {
            rotation -= rotation_delta;
        }
        else if (stick.x <= -STICK_THRESHOLD) {
            rotation += rotation_delta;
        }

        quat_set_axis_angle(&game->player.transform.rotation, 0, 1, 0, rotation * (M_PI / 180.0f));

        Vec3 forward;
        fw64_transform_forward(&game->player.transform, &forward);
        vec3_scale(&forward, &forward, PLAYER_MOVE_SPEED * stick.y * game->engine->time->time_delta);
        vec3_add(&game->player.transform.position, &game->player.transform.position, &forward);
        fw64_node_refresh(&game->player);
    }
}