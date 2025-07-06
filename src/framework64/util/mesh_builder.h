#pragma once

/** \file mesh_builder.h */

#include "framework64/allocator.h"
#include "framework64/asset_database.h"
#include "framework64/texture.h"
#include "framework64/mesh.h"

typedef struct fw64MeshBuilder fw64MeshBuilder;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Creates a mesh builder with the supplied number of primitives and images.
 * A corresponding material is allocated for each primitive.
 * A corresponding texture is allocated for each image.
 * The supplied allocater will be used for all allocations
*/
fw64MeshBuilder* fw64_mesh_builder_create(fw64AssetDatabase* assets, size_t primitive_count, size_t image_count, fw64Allocator* allocator);

/**
 * Resets the mesh builder.
 * This should be called after fw64_mesh_builder_commit if you intend to re-use it
*/
void fw64_mesh_builder_reset(fw64MeshBuilder* mesh_builder, size_t primitive_count, size_t image_count);

/** Deletes all resources currently in use by this builder. */
void fw64_mesh_builder_delete(fw64MeshBuilder* mesh_builder);

/** 
 * Loads the image with the suppplied asset_id into the next slot of the underlying material bundle.
 * If the image fails to load or the maximum number of images have been allocated, returns NULL.
 */
fw64Texture* fw64_mesh_builder_load_image(fw64MeshBuilder* mesh_builder, fw64AssetId asset_id);

/**
 * Returns the material associated with the primivie at the given index.
 * If index is out of bounds, returns NULL.
*/
fw64Material* fw64_mesh_builder_get_material(fw64MeshBuilder* mesh_builder, size_t index);

/**
 * Allocates the underlying vertex data and element indices for Quads at the given index.
 * This is a convenience method for setting up basic grid-like meshes.
 * If you need more fine grained control refer to fw64_mesh_builder_allocate_primitive_data
 * Vertex elements are specified in a counter clockwise winding order.
 * If allocation fails, the data has already been allocated, or the index is out of range returns 0.
*/
int fw64_mesh_builder_allocate_primitive_quad_data(fw64MeshBuilder* mesh_builder, size_t index, fw64VertexAttributes vertex_attributes, size_t count);

/**
 * Allocates the underlying vertex data and element indices for the primitive at the given index.
 * Unline fw64_mesh_builder_allocate_primitive_element_data you will need to specify the vertex indices for each element manually using fw64_mesh_builder_set_triangle_vertex_indices or \ref fw64_mesh_builder_set_line_vertex_indices based on the primitive mode specified to this function.
 * Note for N64-libultra this method is currently limitied to 32 vertices.  If you need more, then you will need to create additional primitives.
 * Vertex elements are expected to be specified in a counter clockwise winding order.
 * If allocation fails, the data has already been allocated, or the index is out of range returns 0.
 */
int fw64_mesh_builder_allocate_primitive_data(fw64MeshBuilder* mesh_builder, size_t index, fw64PrimitiveMode mode, fw64VertexAttributes vertex_attributes, size_t vertex_count, size_t element_count);

/** 
 * Specifies the vertex indices for the triangle with the given index.
 * The primitive should have been allocated with FW64_PRIMITIVE_MODE_TRIANGLES
 */
void fw64_mesh_builder_set_triangle_vertex_indices(fw64MeshBuilder* mesh_builder, size_t index, uint16_t a, uint16_t b, uint16_t c);

/** 
 * Specifies the vertex indices for the line with the given index.
 * The primitive should have been allocated with FW64_PRIMITIVE_MODE_LINES
 */
void fw64_mesh_builder_set_line_vertex_indices(fw64MeshBuilder* mesh_builder, size_t index, uint16_t a, uint16_t b);

/**
 * Sets the active primitive to be used for following set_vertex_xxx commands
 * Returns 0 if the index is out of range
*/
int fw64_mesh_builder_set_active_primitive(fw64MeshBuilder* mesh_builder, size_t index);
void fw64_mesh_builder_set_vertex_position_f(fw64MeshBuilder* mesh_builder, size_t index, float x, float y, float z);
void fw64_mesh_builder_set_vertex_position_int16(fw64MeshBuilder* mesh_builder, size_t index, int16_t x, int16_t y, int16_t z);
void fw64_mesh_builder_set_vertex_normal_f(fw64MeshBuilder* mesh_builder, size_t index, float x, float y, float z);
void fw64_mesh_builder_set_vertex_color_c(fw64MeshBuilder* mesh_builder, size_t index, fw64ColorRGBA8 color);
void fw64_mesh_builder_set_vertex_color_rgba8(fw64MeshBuilder* mesh_builder, size_t index, uint8_t r, uint8_t g, uint8_t b, uint8_t a);

/**
 * Sets the texture coordinates for the vertex at the given index.
 * Note that For N64-libultra, the active primitive's material should have its texture set before calling this function
*/
void fw64_mesh_builder_set_vertex_texcoords_f(fw64MeshBuilder* mesh_builder, size_t index, float s, float t);

/**
 * Manually sets the bounding box of the mesh.
 * If this has not been called before fw64_mesh_builder_commit then the bounding will be calculated.
*/
void fw64_mesh_builder_set_bounding(fw64MeshBuilder* mesh_builder, const Box* bounding);

/**
 * Creates a mesh from the specified data.
 * Note: after this function call the state of the builder will be invalid.
 * Please call fw64_mesh_builder_reset if you intend to use it again
 * This call will not free resources used by the mesh builder.  fw64_mesh_builder_delete should be used when the builder is no longer needed.
*/
fw64Mesh* fw64_mesh_builder_commit(fw64MeshBuilder* mesh_builder);


#ifdef __cplusplus
}
#endif
