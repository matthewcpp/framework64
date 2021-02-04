#ifndef EXAMPLE_OBJECT_H
#define EXAMPLE_OBJECT_H

#include "ultra/entity.h"
#include "ultra/renderer.h"


void wire_object_draw(Entity* entity, Renderer* renderer);
void solid_object_draw(Entity* entity, Renderer* renderer);

void init_consolas(Font* font);

#endif