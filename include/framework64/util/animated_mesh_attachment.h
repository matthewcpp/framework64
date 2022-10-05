#pragma once

/** \file animated_mesh_attachment.h */

#include "framework64/animation_controller.h"
#include "framework64/transform.h"

/**
 * This utility class will compute a world transform matrix for a specific joint node taken as a child of the provided reference transform.
 * For example if you have an animated character, whos world transformation is described by the reference transform,
 * this class could be used to compute the matrix needed to render a sword in it's hand.
 */
typedef struct {
    uint32_t joint_index;
    fw64AnimationController* controller;
    fw64Transform* reference;
    Vec3 local_position;
    Quat local_rotation;
    Vec3 local_scale;
    fw64Matrix matrix;
} fw64AnimatedMeshAttachment;

void fw64_animated_mesh_attachment_init(fw64AnimatedMeshAttachment* attachment, fw64Transform* reference, fw64AnimationController* controller, uint32_t joint_index);
void fw64_animated_mesh_attachment_update(fw64AnimatedMeshAttachment* attachment);