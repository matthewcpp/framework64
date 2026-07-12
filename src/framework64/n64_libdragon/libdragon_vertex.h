#pragma once

#include <stdint.h>

#if 1
/** Vertex format used for drawing sprites */
typedef struct {
    /** S10.5 fixed point format */
    int16_t pos[3];
    int16_t flag;
    /** S10.5 fixed point format */
    int16_t tex_coord[2];
    uint8_t color[4];
} Fw64LibdragonSpriteVertex;
#else
typedef struct {
    float x, y, z;
    float s, t;
} Fw64LibdragonSpriteVertex;
#endif

#define FW64_LIBDRAGON_VERTEX_ALIGNMENT 16
#define FW64_LIBDRAGON_FRACTIONAL_BITS 5
