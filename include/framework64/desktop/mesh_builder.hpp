#pragma once

#include <memory>
#include <limits>

#include "framework64/box.h"

#include "framework64/util/mesh_builder.h"
#include "framework64/desktop/primitive_data.hpp"
#include "framework64/desktop/material_bundle.hpp"
#include "framework64/desktop/mesh.hpp"

struct fw64MeshBuilder {
public:
    struct PrimitiveInfo {
        framework64::PrimitiveData primitive_data;
        fw64Primitive::Mode mode;
        fw64MeshBuilderVertexAttributes vertex_attributes;
    };

public:
    fw64MeshBuilder(fw64AssetDatabase* assets, fw64Allocator* allocator);
    bool setActivePrimitive(size_t index);
    bool allocatePrimitiveQuadData(size_t index, fw64MeshBuilderVertexAttributes vertex_attributes, size_t count);
    bool allocatePrimitiveData(size_t index, fw64Primitive::Mode mode, fw64MeshBuilderVertexAttributes vertex_attributes, size_t vertex_count, size_t element_count);
    fw64Texture* loadImage(fw64AssetId id);

    void reset(size_t primitive_count, size_t image_count);
    fw64Mesh* createMesh();

    fw64AssetDatabase* assets;
    fw64Allocator* allocator;
    std::vector<PrimitiveInfo> primitive_info_vec;
    std::unique_ptr<fw64MaterialBundle> material_bundle;
    Box bounding;
    size_t active_mesh_index;
    size_t allocated_image_count;
};
