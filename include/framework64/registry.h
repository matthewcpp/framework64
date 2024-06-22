#pragma once

#include "framework64/component.h"
#include "framework64/node.h"

typedef void (*fw64ComponentAdded)(fw64Node* node, fw64Component* component, void* arg);

typedef struct {
    int i;
} fw64Registry;
