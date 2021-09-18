#ifndef FW64_TIME_H
#define FW64_TIME_H

#include <stdint.h>

typedef struct {
    float time_delta;
    float total_time;
} fw64Time;

#endif