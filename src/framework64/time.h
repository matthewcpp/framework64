#pragma once

/** \file time.h */

#include <stdint.h>

typedef struct {
    /** Time since the last frame. */
    float time_delta;

    /** The delta time of the fixed update loop. */
    float fixed_time_delta;

    /** Time the application has been running. */
    float total_time;

    float accumulator_progress;
} fw64Time;
