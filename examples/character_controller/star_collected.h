#pragma once

#include "player.h"
#include "star.h"

typedef enum {
    STAR_COLLECTED_STATE_INACTIVE,
    STAR_COLLECTED_STATE_ACTIVE
} StarCollectedState;

typedef struct {
    fw64Engine* engine;
    Player* player;
    Star* star;
    uint32_t music_track;
    StarCollectedState state;
    float elapsed_time;
    Vec3 camera_target_pos, cam_start_pos;
    Vec3 star_target_pos, star_target_scale;
} StarCollected;

void star_collected_init(StarCollected* cutscene, fw64Engine* engine, Player* player, Star* star);
void star_collected_start(StarCollected* cutscene, uint32_t music_track);
void star_collected_update(StarCollected* cutscene, float time_delta);

#define star_collected_is_active(cutscene) ((cutscene)->state == STAR_COLLECTED_STATE_ACTIVE)
