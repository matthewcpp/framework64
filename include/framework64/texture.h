#ifndef FW64_TEXTURE_H
#define FW64_TEXTURE_H

#include <stdint.h>

typedef struct {
    int width;
    int height;
    const unsigned char* data;
} Texture;

#endif