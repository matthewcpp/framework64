#pragma once

#include "framework64/scene.h"

/** 
 * The default partiion scheme does not provide any acceleration.
 * It simply loops over all nodes which have colliders in the scene.
 * */
typedef struct {
    fw64SceneParition base;
    fw64Scene* scene;
} fw64ScenePartitionDefault;

void fw64_scene_partition_default_init(fw64ScenePartitionDefault* partition, fw64Scene* scene);
