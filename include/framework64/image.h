#pragma once

#include <stdint.h>

typedef struct fw64Image fw64Image;

#ifdef __cplusplus
extern "C" {
#endif

void fw64_image_reference_add(fw64Image* image);
void fw64_image_reference_remove(fw64Image* image);

#ifdef __cplusplus
}
#endif