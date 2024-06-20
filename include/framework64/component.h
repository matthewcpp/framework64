#pragma once

#include <stdint.h>

typedef struct fw64Component fw64Component;
typedef uint32_t fw64ComponentTypeId;

struct fw64Component {
    uint32_t type_id;
    fw64Component* next;
};

typedef enum {
    FW64_COMPONENT_TYPE_NONE,
    FW64_COMPONENT_TYPE_MESH_INSTANCE
} fw64BuiltinComponentType;
