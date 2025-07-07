#pragma once

#include <framework64/collision_mesh.h>
#include <framework64/mesh.h>

#ifdef __cplusplus
extern "C" {
#endif

fw64Mesh* fw64_wireframe_build_box(const Box* box, fw64AssetDatabase* assets, fw64Allocator* allocator);

/** Generates a wireframe mesh from a collision mesh.  
 *  Note this method is not the most optimal, so do not use when performance is a concern */
fw64Mesh* fw64_wireframe_build_collision_mesh(const fw64CollisionMesh* collision_mesh, fw64AssetDatabase* assets, fw64Allocator* allocator);

#ifdef __cplusplus
}
#endif
