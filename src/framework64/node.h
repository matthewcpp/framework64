#pragma once

/** \file node.h */

#include "framework64/box.h"
#include "framework64/collider.h"
#include "framework64/mesh_instance.h"
#include "framework64/transform.h"

#include <limits.h>

#ifndef FW64_DEFAULT_LAYER_MASK
#define FW64_DEFAULT_LAYER_MASK 1U
#endif

struct fw64Node {
    fw64Transform transform;
    fw64Collider* collider;
    fw64MeshInstance* mesh_instance;
    uintptr_t data;
    uint32_t layer_mask;

    #ifdef FW64_PLATFORM_IS_64_BIT
    uint32_t _align;
    #endif
};

#ifdef __cplusplus
extern "C" {
#endif

void fw64_node_init(fw64Node* node);
void fw64_node_update(fw64Node* node);

#ifdef __cplusplus
}
#endif
