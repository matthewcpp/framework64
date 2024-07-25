#pragma once

#include "framework64/color.h"

#include <nusys.h>

typedef union {
    Lights1 l1;
    Lights2 l2;
} N64LightingState;

typedef struct {
    u16 color[3];
    u16 active;
    s8 direction[3];
    s8 padding;
} N64LightInfo;



