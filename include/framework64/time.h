#ifndef FW64_TIME_H
#define FW64_TIME_H

#include <ultra64.h>

typedef struct {
    u64 _previous_ms;
    float time_delta;
} fw64Time;

void fw64_time_init(fw64Time* time);
void fw64_time_update(fw64Time* time);

#endif