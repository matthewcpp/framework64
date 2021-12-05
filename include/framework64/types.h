#pragma once

#ifdef PLATFORM_N64
#include <nusys.h>

#include <malloc.h>
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
#define fw64_acosf acosf
#define fw64_fabsf fabsf

#define fw64_malloc malloc
#define fw64_memalign memalign
#define fw64_realloc realloc
#define fw64_free free

#else
typedef struct {
    float m[16];
} fw64Matrix;


#include <float.h>
#include <math.h>
#include <stdlib.h>

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

#define fw64_sinf sinf
#define fw64_cosf cosf
#define fw64_tanf tanf
#define fw64_sqrtf sqrtf
#define fw64_acosf acosf
#define fw64_fabsf fabsf

#define fw64_malloc malloc
#define fw64_memalign(align, size) malloc((size))
#define fw64_realloc realloc
#define fw64_free free

#endif

#ifndef EPSILON
    #define EPSILON 0.000001f
#endif
