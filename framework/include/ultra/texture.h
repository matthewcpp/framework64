#ifndef TEXTURE_H
#define TEXTURE_H

#include <stdint.h>

typedef struct {
    int width;
    int height;
    const unsigned char* data;
} Texture;

Texture* texture_load(int index);
void texture_destroy(Texture* texture);
#endif