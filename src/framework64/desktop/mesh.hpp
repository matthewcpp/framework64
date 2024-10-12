#pragma once

#include "framework64/mesh.h"

#include "framework64/allocator.h"
#include "framework64/data_io.h"

#include "framework64/desktop/material.hpp"
#include "framework64/desktop/material_bundle.hpp"

#include "framework64/desktop/primitive_data.hpp"

#ifdef __linux__
#include <GL/glew.h>
#else
#include <gl/glew.h>
#endif

#include <array>
#include <memory>
#include <string>
#include <vector>

namespace framework64{
    class ShaderCache;
};

struct fw64Primitive {
    enum class Mode { 
        Unknown = 0,
        Triangles = GL_TRIANGLES, 
        Lines = GL_LINES 
    };

    // note this should be kept in sync with pipeline/desktop/MeshWriter.js
    enum Attributes { 
        Positions = 1, 
        Normals = 2, 
        TexCoords = 4, 
        VertexColors = 8 
    };

    Mode mode = Mode::Unknown;
    uint32_t joint_index = FW64_JOINT_INDEX_NONE;
    Box bounding_box;

    framework64::PrimitiveData primitive_data;
    framework64::GLMeshInfo gl_info;
};

struct fw64Mesh {
    fw64Mesh();
    ~fw64Mesh();

    std::unique_ptr<fw64MaterialBundle> material_bundle;
    std::vector<fw64Primitive> primitives;
    Box bounding_box;

    fw64MaterialCollection material_collection;

    /// *moves* pdata into the new primitive and creates the GL objects
    /// TODO: joint index
    fw64Primitive& createPrimitive(framework64::PrimitiveData&& data, fw64Primitive::Mode primitive_mode, Box const & primitive_bounding);
    fw64Primitive& createPrimitive(framework64::PrimitiveData&& data, fw64Primitive::Mode primitive_mode);

    static fw64Mesh* loadFromDatasource(fw64DataSource* data_source, fw64MaterialBundle* material_bundle, framework64::ShaderCache& shader_cache, fw64Allocator* allocator);
};
