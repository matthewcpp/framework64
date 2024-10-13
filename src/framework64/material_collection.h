#pragma once

/** \file collider.h */

#include "framework64/allocator.h"
#include "framework64/material.h"

#include <stdint.h>

/** 
 * Represents a collection of materials used to render a mesh. 
 * Each material in the material set corresponds to a primitive in a mesh.
 * */
typedef struct {
    uint32_t size;
    fw64Material** materials;
} fw64MaterialCollection;

#ifdef __cplusplus
extern "C" {
#endif


/** 
 * Creates a new, empty material collection with the supplied capacity. 
 * Size will be set to zero and material pointers NULL.
 * */
void fw64_material_collection_init_empty(fw64MaterialCollection* collection, uint32_t size, fw64Allocator* allocator);

/**
 * Clones an existing material collection.
 * Note this will make COPIES of each material in the source set.
 * This will result in multiple copies of the same material if one is shared among multiple primitives in the source set.
 */
void fw64_material_collection_init_clone(fw64MaterialCollection* collection, const fw64MaterialCollection* source, fw64Allocator* allocator);

void fw64_material_collection_uninit(fw64MaterialCollection* collection, fw64Allocator* allocator);

#ifdef __cplusplus
}
#endif


/** Returns a pointer to the material at the given index */
#define fw64_material_collection_get_material(material_set, index) ((material_set)->materials[(index)])

/** Sets the material at the given index */
#define fw64_material_collection_set_material(material_collection, index, material) ((material_collection)->materials[(index)] = (material))
