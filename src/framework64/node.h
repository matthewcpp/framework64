#pragma once

/** \file node.h */

#include "framework64/box.h"
#include "framework64/collider.h"
#include "framework64/mesh_instance.h"
#include "framework64/transform.h"

#include <stdint.h>

typedef uint32_t fw64LayerMask;
#define FW64_LAYER_MASK_ALL_LAYERS (~0U)

#ifndef FW64_DEFAULT_LAYER_MASK
#define FW64_DEFAULT_LAYER_MASK 1U
#endif

struct fw64Node {
    fw64Transform transform;
    fw64Collider* collider;
    fw64MeshInstance* mesh_instance;
    uintptr_t data;
    fw64LayerMask layer_mask;

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
