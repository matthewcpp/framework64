#pragma once

#ifdef PLATFORM_N64
#include <nusys.h>

#include <math.h>

#include "framework64/n64/compat.h"

typedef Mtx fw64Matrix;

#ifndef M_PI
    #define M_PI FVAL_PI
#endif

#define fw64_sinf _nsinf
#define fw64_cosf _ncosf
#define fw64_tanf tanf
#define fw64_sqrtf _nsqrtf

#else
typedef struct {
    float m[16];
} fw64Matrix;


#include <math.h>
#include <float.h>

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

#define fw64_sinf sinf
#define fw64_cosf cosf
#define fw64_tanf tanf
#define fw64_sqrtf sqrtf

#endif

#ifndef EPSILON
    #define EPSILON 0.000001f
#endif
