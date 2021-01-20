#include "ultra/texture.h"

#include "n64_logo_sprite.h"

#include <stddef.h>
#include <malloc.h>
#include <string.h>


Texture* texture_load(int index) {
    switch(index) {
        case 1: {
            Texture* texture = malloc(sizeof(Texture));
            texture->width = n64_logo_sprite_info[0];
            texture->height = n64_logo_sprite_info[1];
            texture->data = n64_logo_sprite_image;
            return texture;
        }
        default:
            return NULL;
    }
}

void texture_destroy(Texture* texture) {
    free(texture);
}
