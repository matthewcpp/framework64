#include "star_collected.h"

#include "assets/character_animation.h"

#include <framework64/math.h>

#define CAM_DURATION 0.33f
#define STAR_DURATION 1.45f

void star_collected_init(StarCollected* cutscene, fw64Engine* engine, Player* player, Star* star) {
    cutscene->engine = engine;
    cutscene->player = player;
    cutscene->star = star;
    cutscene->state = STAR_COLLECTED_STATE_INACTIVE;
}

void star_collected_start(StarCollected* cutscene, uint32_t music_track) {
    player_set_character_control_enabled(cutscene->player, 0);
    fw64_audio_play_music(cutscene->engine->audio, music_track);

    fw64SkinnedMeshInstance* skinned_mesh_instance = (fw64SkinnedMeshInstance*)cutscene->player->node->mesh_instance;
    fw64_animation_controller_set_animation(&skinned_mesh_instance->controller, character_animation_Dance);
    skinned_mesh_instance->controller.loop = 1;
    fw64_animation_controller_play(&skinned_mesh_instance->controller);

    // setup the camera transition positions
    cutscene->cam_start_pos = cutscene->player->camera.node->transform.position;
    Vec3 forward;
    fw64_transform_back(&cutscene->player->node->transform, &forward);
    vec3_add_and_scale(&cutscene->player->node->transform.position, &forward, 5.0f, &cutscene->camera_target_pos);
    cutscene->camera_target_pos.y += cutscene->player->character.size.y;

    // setup the star animation values
    Vec3 extents;
    box_extents(&star_get_node(cutscene->star)->mesh_instance->render_bounds, &extents);
    cutscene->star_target_pos = cutscene->player->node->transform.position;
    cutscene->star_target_pos.y += cutscene->player->character.size.y + extents.y + 0.25f;
    cutscene->star_target_scale = star_get_node(cutscene->star)->transform.scale;

    cutscene->elapsed_time = 0.0f;
    cutscene->state = STAR_COLLECTED_STATE_ACTIVE;
}

void star_collected_update(StarCollected* cutscene, float time_delta) {
    //move the camera towards the target
    cutscene->elapsed_time += time_delta;
    const float ct = fw64_minf(cutscene->elapsed_time / CAM_DURATION, 1.0f);

    // animatie camera position
    Vec3* camera_pos = &cutscene->player->camera.node->transform.position;
    vec3_smoothstep(&cutscene->cam_start_pos, &cutscene->camera_target_pos, ct, camera_pos);
    Vec3 player_head = cutscene->player->node->transform.position;
    player_head.y += cutscene->player->character.size.y;
    
    Vec3 up = {0.0f, 1.0f, 0.0f};
    fw64_transform_look_at(&cutscene->player->camera.node->transform, &player_head, &up);
    fw64_camera_update_view_matrix(&cutscene->player->camera);

    //animate the star
    fw64Node* star_node = star_get_node(cutscene->star);
    Vec3 zero = vec3_zero();
    const float st = fw64_minf(cutscene->elapsed_time / STAR_DURATION, 1.0f);
    vec3_smoothstep(&cutscene->player->node->transform.position, &cutscene->star_target_pos, st, &star_node->transform.position);
    vec3_smoothstep(&zero, &cutscene->star_target_scale, st, &star_node->transform.scale);

    // todo should this be here?
    fw64_skinned_mesh_instance_update((fw64SkinnedMeshInstance*)cutscene->player->node->mesh_instance, cutscene->engine->time->time_delta);
}
