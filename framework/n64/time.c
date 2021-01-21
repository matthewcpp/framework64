#include "ultra/time.h"

void time_init(Time* time){
    time->_previous_ms = 0;
    time->time_delta = 0.0f;
}

void time_update(Time* time) {
    u64 current_ms = OS_CYCLES_TO_USEC(osGetTime()) / 1000;

    if (current_ms < time->_previous_ms)
        return;

    float ms_delta = (float)(current_ms - time->_previous_ms);
    time->time_delta = ms_delta / 1000.0f;

    time->_previous_ms = current_ms;
}
