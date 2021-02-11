#ifndef FW64_TIME_H
#define FW64_TIME_H

#include <ultra64.h>

typedef struct {
    u64 _previous_ms;
    float time_delta;
} Time;

void time_init(Time* time);
void time_update(Time* time);

#endif