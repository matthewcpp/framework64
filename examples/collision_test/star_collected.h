#pragma once

#include "player.h"

typedef enum {
    STAR_COLLECTED_STATE_INACTIVE,
    STAR_COLLECTED_STATE_ACTIVE
} StarCollectedState;

typedef struct {
    fw64Engine* engine;
    Player* player;
    uint32_t music_track;
    StarCollectedState state;
} StarCollected;

void star_collected_init(StarCollected* cutscene, fw64Engine* engine, Player* player);
void star_collected_start(StarCollected* cutscene, uint32_t music_track);
void star_collected_update(StarCollected* cutscene);

#define star_collected_is_active(cutscene) ((cutscene)->state == STAR_COLLECTED_STATE_ACTIVE)
