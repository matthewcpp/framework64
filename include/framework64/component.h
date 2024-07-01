#pragma once

/** \file component.h */

#include <stdint.h>

typedef struct fw64Node fw64Node;

typedef struct fw64Component fw64Component;
typedef uint32_t fw64ComponentTypeId;

struct fw64Component {
    uint32_t type_id;
    fw64Node* node;
};

typedef struct {
    uint32_t type_id;
} fw64Components;
