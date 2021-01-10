#ifndef STATIC_MODEL_H
#define STATIC_MODEL_H

#include "box.h"
#include <nusys.h>

void static_model_bounding_box(int handle, Box* box);
int static_model_render(int handle, Gfx* display_list);

#endif