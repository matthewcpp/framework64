#ifndef STATIC_MODEL_H
#define STATIC_MODEL_H

#include "box.h"
#include "renderer.h"

#include <nusys.h>

void static_model_bounding_box(int handle, Box* box);
void static_model_render(int handle, Renderer* renderer);

#endif