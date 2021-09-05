#include "framework64/desktop/glb_parser.h"

#include "framework64/matrix.h"

#include <limits>

namespace framework64 {

constexpr uint32_t HeaderMagicNumber = 0x46546C67;
constexpr uint32_t JsonChunkMagicNumber = 0x4E4F534A;
constexpr uint32_t BinaryChunkMagicNumber = 0x004E4942;

enum GltfComponentType {
    UnsignedShort = 5123,
    UnsignedInt = 5125
};

// note this function assumes scene 0 is the "active scene"
fw64Mesh* GlbParser::parseStaticMesh(std::string const & path) {
    glb_file.open(path, std::ios::binary);

    if (!glb_file)
        return nullptr;

    if (!parseHeader() || !parseJsonChunk() || !parseBinaryChunk() )
        return nullptr;

    return createStaticMesh(json_doc["meshes"][0]);
}

bool GlbParser::parseHeader() {
    glb_file.read(reinterpret_cast<char * >(&header), sizeof(Header));

    return header.magic == HeaderMagicNumber && header.version == 2;
}

bool GlbParser::parseJsonChunk() {
    glb_file.read(reinterpret_cast<char * >(&json_chunk_info), sizeof(ChunkInfo));

    if (json_chunk_info.type != JsonChunkMagicNumber)
        return false;

    std::vector<char> json_data(json_chunk_info.length);
    glb_file.read(json_data.data(), json_chunk_info.length);

    json_doc = nlohmann::json ::parse(json_data.begin(), json_data.end());

    return true;
}

bool GlbParser::parseBinaryChunk() {
    glb_file.read(reinterpret_cast<char * >(&binary_chunk_info), sizeof(ChunkInfo));

    return binary_chunk_info.type == BinaryChunkMagicNumber;
}

Box GlbParser::getBoxFromAccessor(size_t accessor_index) const {
    auto const & accessor_node = json_doc["accessors"][accessor_index];
    auto const & min_node = accessor_node["min"];
    auto const & max_node = accessor_node["max"];

    Box box;
    box.min.x = min_node[0].get<float>();
    box.min.y = min_node[1].get<float>();
    box.min.z = min_node[2].get<float>();

    box.max.x = max_node[0].get<float>();
    box.max.y = max_node[1].get<float>();
    box.max.z = max_node[2].get<float>();

    return box;
}

static fw64Mesh::Primitive::Mode getPrimitiveMode(nlohmann::json const & primitive_node) {
    if (!primitive_node.contains("mode"))
        return fw64Mesh::Primitive::Mode::Triangles;
    
    int mode = primitive_node["mode"].get<int>();

    switch (mode) {
        case 1:
            return fw64Mesh::Primitive::Mode::Lines;
        case 4:
            return fw64Mesh::Primitive::Mode::Triangles;
        default:
            return fw64Mesh::Primitive::Mode::Unknown;
    }
}

// note this assumes there is only one mesh at the top level
// it will need to be modified in the future
static std::array<float, 16> extractMatrixFromNode(nlohmann::json const & node) {
    std::array<float, 16> transform;
    matrix_set_identity(transform.data());

    Vec3 translation = {0.0f, 0.0f, 0.0f};
    Quat rotation;
    quat_ident(&rotation);
    Vec3 scale = {1.0f, 1.0f, 1.0f};

    bool set_trs = true;

    if (node.contains("translation")) {
        auto const & translation_node = node["translation"];
        translation.x = translation_node[0].get<float>();
        translation.y = translation_node[1].get<float>();
        translation.z = translation_node[2].get<float>();
    }

    if (node.contains("rotation")) {
        auto const & rotation_node = node["rotation"];
        rotation.x = rotation_node[0].get<float>();
        rotation.y = rotation_node[1].get<float>();
        rotation.z = rotation_node[2].get<float>();
        rotation.w = rotation_node[3].get<float>();
    }

    if (node.contains("scale")) {
        auto const & scale_node = node["scale"];
        scale.x = scale_node[0].get<float>();
        scale.y = scale_node[1].get<float>();
        scale.z = scale_node[2].get<float>();
    }

    if (node.contains("matrix")) {
        auto const &matrix_node = node["matrix"];

        for (size_t i = 0; i < 16; i++) {
            transform[i] = matrix_node[i].get<float>();
        }

        set_trs = false;
    }

    if (set_trs) {
        matrix_from_trs(transform.data(), &translation, &rotation, &scale);
    }

    return transform;
}

fw64Mesh* GlbParser::createStaticMesh(nlohmann::json const & mesh_node) {
    auto mesh = new fw64Mesh();

    for (auto const & primitive_node : mesh_node["primitives"]) {
        auto const primitive_mode = getPrimitiveMode(primitive_node);

        if (primitive_mode == fw64Mesh::Primitive::Mode::Unknown)
            continue;

        auto & primitive = mesh->primitives.emplace_back();

        primitive.mode = primitive_mode;

        if (primitive_node.contains("material"))
            parseMaterial(primitive.material, primitive_node["material"].get<size_t>());

        auto const & attributes_node = primitive_node["attributes"];

        std::vector<float> positions = readPrimitiveAttributeBuffer<float>(attributes_node, "POSITION");
        std::vector<float> normals = readPrimitiveAttributeBuffer<float>(attributes_node, "NORMAL");
        std::vector<float> tex_coords = readPrimitiveAttributeBuffer<float>(attributes_node, "TEXCOORD_0");
        std::vector<float> colors = readPrimitiveAttributeBuffer<float>(attributes_node, "COLOR_0");

        GLuint array_buffer_size = (positions.size() + normals.size() + tex_coords.size() + colors.size()) * sizeof(float);
        glGenVertexArrays(1, &primitive.gl_vertex_array_object);
        glBindVertexArray(primitive.gl_vertex_array_object);

        glGenBuffers(1, &primitive.gl_array_buffer_object);
        glBindBuffer(GL_ARRAY_BUFFER, primitive.gl_array_buffer_object);
        glBufferData(GL_ARRAY_BUFFER, array_buffer_size, nullptr, GL_STATIC_DRAW);
        GLsizeiptr buffer_offset = 0;

        if (!positions.empty()) {
            auto accessor = attributes_node["POSITION"].get<size_t>();
            primitive.bounding_box = getBoxFromAccessor(accessor);
            box_encapsulate_box(&mesh->bounding_box, &primitive.bounding_box);
            primitive.attributes |= fw64Mesh::Primitive::Attributes::Positions;
            GLsizeiptr data_size = positions.size() * sizeof(float);
            glBufferSubData(GL_ARRAY_BUFFER, buffer_offset, data_size, positions.data());
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)buffer_offset);
            buffer_offset += data_size;
        }

        if (!normals.empty()) {
            GLsizeiptr data_size = normals.size() * sizeof(float);
            glBufferSubData(GL_ARRAY_BUFFER, buffer_offset, data_size, normals.data());
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)buffer_offset);
            buffer_offset += data_size;
            primitive.attributes |= fw64Mesh::Primitive::Attributes::Normals;
        }

        if (!tex_coords.empty()) {
            GLsizeiptr data_size = tex_coords.size() * sizeof(float);
            glBufferSubData(GL_ARRAY_BUFFER, buffer_offset, data_size, tex_coords.data());
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)buffer_offset);
            buffer_offset += data_size;
            primitive.attributes |= fw64Mesh::Primitive::Attributes::TexCoords;
        }

        // note that we are assuming vertex colors are encoded as RGB floats (seems to be default blender export)
        if (!colors.empty()) {
            GLsizeiptr data_size = colors.size() * sizeof(float);
            glBufferSubData(GL_ARRAY_BUFFER, buffer_offset, data_size, colors.data());
            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void*)buffer_offset);
            buffer_offset += data_size;
            primitive.attributes |= fw64Mesh::Primitive::Attributes::VertexColors;
        }

        parseIndices(primitive, primitive_node);

        glBindVertexArray(0);

        primitive.material.shader = shader_cache.getShaderProgram(primitive);
    }

    return mesh;
}

void GlbParser::parseMaterial(Material& material, size_t material_index) {
    auto const & material_node = json_doc["materials"][material_index];
    auto const & pbr = material_node["pbrMetallicRoughness"];
    auto const & base_color_factor = pbr["baseColorFactor"];

    for (size_t i = 0; i < 4; i++) {
        material.color[i] = base_color_factor[i].get<float>();
    }
}

void GlbParser::parseIndices(fw64Mesh::Primitive& primitive, nlohmann::json const & primitive_node) {
    auto const & element_accessor_node = json_doc["accessors"][primitive_node["indices"].get<size_t>()];
    auto componentType = element_accessor_node["componentType"].get<uint32_t>();
    auto bufferViewIndex = element_accessor_node["bufferView"].get<size_t>();

    if (componentType == GltfComponentType::UnsignedShort) {
        primitive.gl_array_buffer_object = readIndicesIntoGlBuffer<uint16_t >(bufferViewIndex);
        primitive.element_type = GL_UNSIGNED_SHORT;
    }

    else if (componentType == GltfComponentType::UnsignedInt) {
        primitive.gl_array_buffer_object = readIndicesIntoGlBuffer<uint32_t >(bufferViewIndex);
        primitive.element_type = GL_UNSIGNED_INT;
    }
    else {
        return; // error?
    }

    primitive.element_count = element_accessor_node["count"].get<GLsizei>();
}

void GlbParser::seekInBinaryChunk(size_t pos) {
    size_t binary_start = sizeof(Header) + sizeof(json_chunk_info) + json_chunk_info.length + sizeof(binary_chunk_info);
    glb_file.seekg(binary_start + pos);
}

}
