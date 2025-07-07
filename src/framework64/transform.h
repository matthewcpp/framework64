#pragma once

/** \file transform.h */

#include "framework64/box.h"
#include "framework64/quat.h"
#include "framework64/types.h"
#include "framework64/vec3.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct fw64Transform fw64Transform;

struct fw64Transform {
    Vec3 position;
    Quat rotation;
    Vec3 scale;
    fw64Transform* parent;
    fw64Transform* first_child;
    fw64Transform* next_sibling;
    float world_matrix[16];
};

void fw64_transform_init(fw64Transform* transform);

/** 
 * Sets the local position, rotation, and scale to their default values.
 * Note this does not recompute the matrix or affect the hierarchy.
 * Use fw64_transform_update_matrix after this call to recompute the matrix.
 * */
void fw64_transform_reset(fw64Transform* transform);
void fw64_transform_forward(const fw64Transform* transform, Vec3* out);
void fw64_transform_back(const fw64Transform* transform, Vec3* out);
void fw64_transform_up(const fw64Transform* transform, Vec3* out);
void fw64_transform_down(const fw64Transform* transform, Vec3* out);
void fw64_transform_left(const fw64Transform* transform, Vec3* out);
void fw64_transform_right(const fw64Transform* transform, Vec3* out);

void fw64_transform_look_at(fw64Transform* transform, const Vec3* target,const  Vec3* up);

/** Updates the matrix for this transform with regards to it's parent.
 *  Additionally this function will recursively update matrices of all descendants
 * */
void fw64_transform_update_matrix(fw64Transform* transform);
void fw64_transform_xform_box(const fw64Transform* transform, const Box* source, Box* target);

void fw64_transform_mult_point(const fw64Transform* transform, const Vec3* point, Vec3* out);
void fw64_transform_inv_mult_point(const fw64Transform* transform, const Vec3* point, Vec3* out);

/** Adds a new child to this transform.
 *  Removed the child from it's existing parent (if any)
 *  Important Note: this function will NOT call fw64_transform_update_matrix on the child after the parenting operation.
 *  Ensure that the method is called once all parenting operations are complete to ensure world matrices are updated.
 */
void fw64_transform_add_child(fw64Transform* parent, fw64Transform* child);

/** 
 * Removes the child from the node's child list.
 * After the call to this function the child's next_sibling value will be NULL.
 * Its own children are not affected
 * Returns non-zero value if the operation was successfull */
int fw64_transform_remove_child(fw64Transform* parent, fw64Transform* child);

#ifdef __cplusplus
}
#endif

