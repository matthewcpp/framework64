#include "star_collected.h"

#include "assets/character_animation.h"

void star_collected_init(StarCollected* cutscene, fw64Engine* engine, Player* player) {
    cutscene->engine = engine;
    cutscene->player = player;
    cutscene->state = STAR_COLLECTED_STATE_INACTIVE;
}

void star_collected_start(StarCollected* cutscene, uint32_t music_track) {
    player_set_character_control_enabled(cutscene->player, 0);
    fw64_audio_play_music(cutscene->engine->audio, music_track);

    fw64SkinnedMeshInstance* skinned_mesh_instance = (fw64SkinnedMeshInstance*)cutscene->player->node->mesh_instance;
    fw64_animation_controller_set_animation(&skinned_mesh_instance->controller, character_animation_Dance);
    skinned_mesh_instance->controller.loop = 1;
    fw64_animation_controller_play(&skinned_mesh_instance->controller);

    cutscene->state = STAR_COLLECTED_STATE_ACTIVE;
}

void star_collected_update(StarCollected* cutscene) {
    // todo should this be here?
    fw64_skinned_mesh_instance_update((fw64SkinnedMeshInstance*)cutscene->player->node->mesh_instance, cutscene->engine->time->time_delta);
}
